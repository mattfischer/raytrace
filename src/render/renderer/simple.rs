use core::f32;

use crate::geo;
use crate::object;
use crate::render;

use geo::Point2;

use object::sampler::Halton;
use object::Radiance;
use object::Sampler;
use object::Scene;

use render::Executor;
use render::ExecutorJob;
use render::Framebuffer;
use render::Lighter;
use render::Raster;
use render::RasterJob;
use render::Renderer;

use std::collections::VecDeque;
use std::sync::Arc;
use std::sync::Mutex;
use std::time::Instant;

pub struct SimpleSettings {
    pub width: usize,
    pub height: usize,
    pub samples: usize,
}

struct Inner {
    scene: Arc<Scene>,
    settings: SimpleSettings,
    lighter: Option<Box<dyn Lighter>>,
    jobs: Mutex<VecDeque<Box<dyn ExecutorJob>>>,
    framebuffer: Arc<Mutex<Framebuffer>>,
    total_radiance: Mutex<Raster<Radiance>>,
    executor: Executor,
    start_time: Mutex<Instant>,
    done_listener: Mutex<Option<Box<dyn FnOnce(f32) + 'static + Send + Sync>>>,
}

pub struct Simple {
    inner: Arc<Inner>,
}

struct ThreadLocal {
    sampler: Halton,
}

impl Simple {
    pub fn new(
        scene: Arc<Scene>,
        settings: SimpleSettings,
        lighter: Option<Box<dyn Lighter>>,
    ) -> Simple {
        let width = settings.width;
        let height = settings.height;
        let samples = settings.samples;

        let framebuffer = Arc::new(Mutex::new(Framebuffer::new(settings.width, settings.height)));
        let total_radiance = Mutex::new(Raster::new(settings.width, settings.height));

        let mut jobs = VecDeque::new();

        if let Some(lighter) = &lighter {
            jobs.extend(lighter.create_prerender_jobs(scene.clone(), framebuffer.clone()));
        }

        let jobs = Mutex::new(jobs);

        let executor = Executor::new();
        let start_time = Mutex::new(Instant::now());
        let done_listener = Mutex::new(None);
        let inner = Arc::new(Inner {
            framebuffer,
            scene,
            settings,
            lighter,
            total_radiance,
            jobs,
            executor,
            done_listener,
            start_time,
        });

        let inner_clone = inner.clone();
        let job = Box::new(RasterJob::new(
            width,
            height,
            samples,
            move |x, y, sample, thread_local: &mut ThreadLocal| {
                inner_clone.render_pixel(x, y, sample, &mut thread_local.sampler);
            },
            move || {
                let sampler = Halton::new(width as i32, height as i32);
                return Box::new(ThreadLocal { sampler });
            },
        ));

        if let Ok(mut jobs) = inner.jobs.lock() {
            jobs.push_back(job as Box<dyn ExecutorJob>);
        }

        return Simple { inner };
    }
}

impl Inner {
    fn render_pixel(
        &self,
        x: usize,
        y: usize,
        sample: usize,
        sampler: &mut dyn Sampler,
    ) {
        sampler.start_sample_with_xys(x, y, sample);
        let image_point = Point2::new(x as f32, y as f32) + sampler.get_value2();
        let aperture_point = sampler.get_value2();
        let beam = self.scene.camera.create_pixel_beam(
            image_point,
            self.settings.width,
            self.settings.height,
            aperture_point,
        );

        let isect = self.scene.intersect(beam, f32::MAX, true);

        let color;
        if let Some(lighter) = &self.lighter {
            let rad;
            if let Some(isect) = isect {
                rad = lighter.light(&isect, sampler);
            } else {
                rad = self.scene.sky_radiance;
            }

            let rad_total;
            if let Ok(mut total_radiance) = self.total_radiance.lock() {
                rad_total = total_radiance.get(x, y) + rad;
                total_radiance.set(x, y, rad_total);
            } else {
                rad_total = Radiance::ZERO;
            }
            color = Framebuffer::tone_map(rad_total / (sample as f32 + 1.0));
        } else {
            if let Some(isect) = isect {
                color = isect.albedo;
            } else {
                color = Framebuffer::tone_map(self.scene.sky_radiance);
            }
        }

        if let Ok(mut framebuffer) = self.framebuffer.lock() {
            framebuffer.set_pixel(x, y, color);
        }
    }

    fn job_done(self: &Arc<Inner>) {
        if let Ok(mut jobs) = self.jobs.lock() {
            if let Some(job) = jobs.pop_front() {
                let self_1 = self.clone();
                self.executor.run_job(job, move || self_1.job_done());
            } else {
                let start_time = if let Ok(s) = self.start_time.lock() {
                    *s
                } else {
                    Instant::now()
                };
                let end_time = Instant::now();
                let time_elapsed = end_time - start_time;
                if let Ok(mut done_listener) = self.done_listener.lock() {
                    if let Some(done_listener) = done_listener.take() {
                        done_listener(time_elapsed.as_secs_f32());
                    }
                }
            }
        }
    }
}

impl Renderer for Simple {
    fn start(&self, done: Box<dyn FnOnce(f32) + 'static + Send + Sync>)
    {
        if let Ok(mut done_listener) = self.inner.done_listener.lock() {
            done_listener.replace(done);
        }

        if let Ok(mut jobs) = self.inner.jobs.lock() {
            if let Some(job) = jobs.pop_front() {
                let inner = self.inner.clone();
                self.inner
                    .executor
                    .run_job(job, move || inner.job_done());
            }
        }

        if let Ok(mut start_time) = self.inner.start_time.lock() {
            *start_time = Instant::now();
        }
    }

    fn stop(&self) {
        self.inner.executor.stop();
    }

    fn running(&self) -> bool {
        return self.inner.executor.running();
    }

    fn framebuffer_ptr(&self) -> *const u8 {
        if let Ok(framebuffer) = self.inner.framebuffer.lock() {
            return framebuffer.bits.as_ptr();
        } else {
            return std::ptr::null();
        }
    }
}