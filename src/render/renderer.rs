use core::f32;

use crate::geo;
use crate::object;
use crate::render;

use geo::Point2;

use object::Radiance;
use object::Sampler;
use object::sampler::Halton;
use object::Scene;

use render::Executor;
use render::ExecutorJob;
use render::Framebuffer;
use render::Lighter;
use render::Raster;
use render::RasterJob;

use std::sync::Arc;
use std::sync::Mutex;
use std::collections::VecDeque;
use std::time::Instant;

pub struct RendererSettings {
    pub width : usize,
    pub height : usize,
    pub samples : usize
}

struct SharedState {
    scene : Scene,
    settings : RendererSettings,
    lighter : Option<Box<dyn Lighter>>,
    jobs : Mutex<VecDeque<Box<dyn ExecutorJob>>>,
    framebuffer : Mutex<Framebuffer>,
    total_radiance : Mutex<Raster<Radiance>>,
    executor : Executor,
    start_time : Mutex<Instant>,
    done_listener : Mutex<Option<Box<dyn FnOnce(f32) + 'static + Send + Sync>>>
}

pub struct Renderer {
    shared_state : Arc<SharedState>
}

struct ThreadLocal {
    sampler : Halton
}

impl Renderer {
    pub fn new(scene : Scene, settings : RendererSettings, lighter : Option<Box<dyn Lighter>>) -> Renderer {
        let width = settings.width;
        let height = settings.height;
        let samples = settings.samples;

        let framebuffer = Mutex::new(Framebuffer::new(settings.width, settings.height));
        let total_radiance = Mutex::new(Raster::new(settings.width, settings.height));

        let jobs = Mutex::new(VecDeque::new());

        let executor = Executor::new();
        let start_time = Mutex::new(Instant::now());
        let done_listener = Mutex::new(None);
        let shared_state = Arc::new(SharedState{framebuffer, scene, settings, lighter, total_radiance, jobs, executor, done_listener, start_time});

        let shared_state_clone = shared_state.clone();
        let job = Box::new(RasterJob::new(width, height, samples,
            move |x, y, sample, thread_local: &mut ThreadLocal| {
                Self::render_pixel(&shared_state_clone, x, y, sample, &mut thread_local.sampler);
            },
            || {
            },
            move || {
                let sampler = Halton::new(width as i32, height as i32);
                return Box::new(ThreadLocal {sampler});
            }));

        if let Ok(mut jobs) = shared_state.jobs.lock() {
            jobs.push_back(job as Box<dyn ExecutorJob>);
        }

        return Renderer {shared_state};
    }

    pub fn start<F>(&self, done : F)
    where F: FnOnce(f32) + 'static + Send + Sync {
        if let Ok(mut done_listener) = self.shared_state.done_listener.lock() {
            done_listener.replace(Box::new(done));
        }

        if let Ok(mut jobs) = self.shared_state.jobs.lock() {
            if let Some(job) = jobs.pop_front() {
                let shared_state_clone = self.shared_state.clone();
                self.shared_state.executor.run_job(job, move || Self::job_done(shared_state_clone));
            }
        }

        if let Ok(mut start_time) = self.shared_state.start_time.lock() {
            *start_time = Instant::now();
        }
    }

    pub fn stop(&self) {
        self.shared_state.executor.stop();
    }

    pub fn running(&self) -> bool {
        return self.shared_state.executor.running();
    }

    pub fn framebuffer_ptr(&self) -> *const u8 {
        if let Ok(framebuffer) = self.shared_state.framebuffer.lock() {
            return framebuffer.bits.as_ptr();
        } else {
            return std::ptr::null();
        }
    }

    pub fn run_with_scene<F, R>(&self, func: F) -> R
    where F: FnOnce(&Scene) -> R {
        let scene = &self.shared_state.scene;
        return func(scene);
    }

    fn render_pixel(data : &SharedState, x: usize, y: usize, sample: usize, sampler : &mut dyn Sampler) {
        sampler.start_sample_with_xys(x, y, sample);
        let image_point = Point2::new(x as f32, y as f32) + sampler.get_value2();
        let aperture_point = sampler.get_value2();
        let beam = data.scene.camera.create_pixel_beam(image_point, data.settings.width, data.settings.height, aperture_point);

        let isect = data.scene.intersect(beam, f32::MAX, true);

        let color;
        if let Some(lighter) = &data.lighter {
            let rad;
            if let Some(isect) = isect {
                rad = lighter.light(&isect, sampler);
            } else {
                rad = data.scene.sky_radiance;
            }

            let rad_total;
            if let Ok(mut total_radiance) = data.total_radiance.lock() {
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
                color = Framebuffer::tone_map(data.scene.sky_radiance);
            }
        }

        if let Ok(mut framebuffer) = data.framebuffer.lock() {
            framebuffer.set_pixel(x, y, color);
        }
    }

    fn job_done(shared_state : Arc<SharedState>) {
        if let Ok(mut jobs) = shared_state.jobs.lock() {
            if let Some(job) = jobs.pop_front() {
                let shared_state_clone = shared_state.clone();
                shared_state.executor.run_job(job, move || Self::job_done(shared_state_clone));
            } else {
                let start_time = if let Ok(s) = shared_state.start_time.lock() { *s } else { Instant::now() };
                let end_time = Instant::now();
                let time_elapsed = end_time - start_time;
                if let Ok(mut done_listener) = shared_state.done_listener.lock() {
                    if let Some(done_listener) = done_listener.take() {
                        done_listener(time_elapsed.as_secs_f32());
                    }
                }
            }
        }
    }
}