use crate::geo;
use crate::object;
use crate::render;

use geo::Beam;
use geo::Bivec3;
use geo::Normal3;
use geo::Point2;
use geo::Point3;
use geo::Ray;
use geo::Vec3;

use object::FlatIntersection;
use object::Intersection;
use object::Radiance;
use object::Sampler;
use object::Scene;
use object::sampler::Halton;

use render::Executor;
use render::Framebuffer;
use render::Lighter;
use render::Raster;
use render::RasterJob;
use render::Renderer;
use render::lighter::UniPath;

use core::f32;
use std::sync::Arc;
use std::sync::Mutex;
use std::sync::RwLock;
use std::time::Instant;

#[derive(Clone, Copy, Default)]
struct Reservoir<T> {
    sample: T,
    w: f32,
    m: usize,
    q: f32 
}

impl<T> Reservoir<T> {
    pub fn add_sample(&mut self, sample: T, q: f32, pdf: f32, sampler: &mut dyn Sampler) {
        self.combine(sample, q, 1.0 / pdf, 1, 1.0, sampler);
    }

    pub fn add_reservoir(&mut self, res: Reservoir<T>, q: f32, j: f32, sampler: &mut dyn Sampler) {
        self.combine(res.sample, q, res.w, res.m, j, sampler);
    }

    fn combine(&mut self, sample_new: T, q: f32, w: f32, m: usize, j: f32, sampler: &mut dyn Sampler) {
        let m0 = (self.m as f32) / ((self.m + m) as f32);
        let m1 = (m as f32) / ((self.m + m) as f32);

        let w0 = m0 * self.q * self.w;
        let w1 = m1 * q * w * j;
        let w_sum = w0 + w1;

        if w0 == 0.0 || sampler.get_value() < w1 / w_sum {
            self.sample = sample_new;
            self.q = q;
        }

        self.m += m;
        self.w = w_sum / self.q;
    }
}

pub struct ReSTIRSettings {
    pub width : usize,
    pub height : usize,
    pub samples : usize,
    pub indirect_samples : usize,
    pub radius : usize,
    pub candidates : usize
}

type DoneFunc = dyn FnOnce(f32) + 'static + Send + Sync;

#[derive(Clone, Copy, Default)]
struct DirectSample {
    radiance: Radiance,
    point: Point3,
    normal: Normal3,
    primitive_idx: usize
}
#[derive(Clone, Copy, Default)]
struct IndirectSample {
    point: Point3,
    normal: Normal3,
    indirect_radiance: Radiance
}

struct Inner {
    scene: Arc<Scene>,
    settings: ReSTIRSettings,
    indirect_lighter: Box<dyn Lighter>,
    primary_hits: RwLock<Raster<Option<FlatIntersection>>>,
    framebuffer: Mutex<Framebuffer>,
    total_radiance: Mutex<Raster<Radiance>>,
    executor: Executor,
    start_time: Mutex<Instant>,
    done_listener: Mutex<Option<Box<DoneFunc>>>,
    current_sample: Mutex<usize>,
    direct_reservoirs: RwLock<Raster<Reservoir<DirectSample>>>,
    indirect_reservoirs: RwLock<Raster<Reservoir<IndirectSample>>>
}

impl Inner {
    fn start_initial_sample_job(self : &Arc<Inner>) {
        let width = self.settings.width;
        let height = self.settings.height;
        let current_sample = match self.current_sample.lock() {
            Ok(current_sample) => *current_sample,
            _ => 0
        };

        let self_1 = self.clone();
        let self_2 = self.clone();

        let job = RasterJob::new(
            width, height, 1,
            move |x, y, _sample, thread_local : &mut ThreadLocal| {
                self_1.initial_sample_pixel(x, y, current_sample, &mut thread_local.sampler);
            },
            move || {
                let sampler = Halton::new(width as i32, height as i32);
                let indirect_samples = Vec::new();
                return Box::new(ThreadLocal { sampler, indirect_samples });
            }
        );
        
        self.executor.run_job(Box::new(job), move || self_2.start_direct_illuminate_job() );
    }

    fn start_direct_illuminate_job(self : &Arc<Inner>) {
        let width = self.settings.width;
        let height = self.settings.height;
        let current_sample = match self.current_sample.lock() {
            Ok(current_sample) => *current_sample,
            _ => 0
        };

        let self_1 = self.clone();
        let self_2 = self.clone();

        let job = RasterJob::new(
            width, height, 1,
            move |x, y, _sample, thread_local : &mut ThreadLocal| {
                self_1.direct_illuminate_pixel(x, y, current_sample, &mut thread_local.sampler);
            },
            move || {
                let sampler = Halton::new(width as i32, height as i32);
                let indirect_samples = Vec::new();
                return Box::new(ThreadLocal { sampler, indirect_samples });
            }
        );
        
        self.executor.run_job(Box::new(job), move || self_2.start_indirect_illuminate_job() );
    }

    fn start_indirect_illuminate_job(self : &Arc<Inner>) {
        let width = self.settings.width;
        let height = self.settings.height;
        let current_sample = match self.current_sample.lock() {
            Ok(current_sample) => *current_sample,
            _ => 0
        };

        let self_1 = self.clone();
        let self_2 = self.clone();
        let self_3 = self.clone();

        let job = RasterJob::new(
            width, height, 1,
            move |x, y, _sample, thread_local : &mut ThreadLocal| {
                self_1.indirect_illuminate_pixel(x, y, current_sample, &mut thread_local.sampler, &mut thread_local.indirect_samples[..]);
            },
            move || {
                let sampler = Halton::new(width as i32, height as i32);
                let mut indirect_samples = Vec::new();
                indirect_samples.resize(self_3.settings.indirect_samples, Default::default());
                return Box::new(ThreadLocal { sampler, indirect_samples });
            }
        );
        
        let done = move || {
            let current_sample = if let Ok(mut current_sample) = self_2.current_sample.lock() {                    
                *current_sample += 1;
                *current_sample
            } else {
                0
            };

            if current_sample < self_2.settings.samples {
                self_2.start_initial_sample_job();
            } else {
                let start_time = if let Ok(s) = self_2.start_time.lock() {
                    *s
                } else {
                    Instant::now()
                };
                let end_time = Instant::now();
                let time_elapsed = end_time - start_time;
                if let Ok(mut done_listener) = self_2.done_listener.lock() {
                    if let Some(done_listener) = done_listener.take() {
                        done_listener(time_elapsed.as_secs_f32());
                    }
                }
            }
        };

        self.executor.run_job(Box::new(job), done);
    }

    fn initial_sample_pixel(&self, x : usize, y : usize, sample : usize, sampler : &mut dyn Sampler) {
        let scene = self.scene.as_ref();

        sampler.start_sample_with_xys(x, y, sample);
        let pnt_image = Point2::new(x as f32, y as f32) + sampler.get_value2();
        let pnt_aperture = sampler.get_value2();
        let beam = scene.camera.create_pixel_beam(pnt_image, self.settings.width, self.settings.height, pnt_aperture);
        
        let rad_emitted;
            
        if let Some(isect) = scene.intersect(beam, f32::MAX, true) {
            if let Ok(mut primary_hits) = self.primary_hits.write() {
                primary_hits.set(x, y, Some(FlatIntersection::from(isect)));
            }

            let nrm_facing = isect.facing_normal;
            let surface = &isect.primitive.surface;
            let pnt_offset = isect.point + Vec3::from(nrm_facing) * 0.01;

            if let Ok(mut direct_reservoirs) = self.direct_reservoirs.write() {
                direct_reservoirs.set(x, y, Default::default());
            }

            for _ in 0..1 {
                let light_index = (sampler.get_value() * (scene.area_lights.len() as f32)) as usize;
                let light = &scene.primitives[scene.area_lights[light_index]];
                let rad2 = light.surface.radiance;

                if let Some((pnt2, nrm2, pdf2)) = light.shape.sample(sampler) {
                    let mut dir_in = pnt2 - pnt_offset;
                    let d = dir_in.mag();
                    dir_in /= d;
                    let dot2 = (dir_in * nrm2).abs();

                    let dot = dir_in * nrm_facing;
                    if dot > 0.0 {
                        let irad = rad2 * dot2 * dot / (d * d);
                        let rad_direct = irad * surface.reflected(&isect, dir_in);
                        let q = rad_direct.mag();

                        let sample = DirectSample {
                            point: pnt2,
                            radiance: rad2,
                            normal: nrm2,
                            primitive_idx: scene.area_lights[light_index]
                        };

                        if let Ok(mut direct_reservoirs) = self.direct_reservoirs.write() {
                            direct_reservoirs.get_mut(x, y).add_sample(sample, q, pdf2, sampler);
                        }
                    }
                }
            }

            if let Ok(mut indirect_reservoirs) = self.indirect_reservoirs.write() {
                indirect_reservoirs.set(x, y, Default::default());
            }

            if let (_reflected, dir_in, Some(pdf)) = surface.sample(&isect, sampler) {
                let reverse = (dir_in * nrm_facing).signum();
                let dot = dir_in * nrm_facing * reverse;

                let pnt_offset = isect.point + Vec3::from(nrm_facing) * 0.01 * reverse;
                if dot > 0.0 {
                    let ray_reflect = Ray::new(pnt_offset, dir_in);
                    let beam = Beam::new(ray_reflect, Bivec3::ZERO, Bivec3::ZERO);
                    if let Some(isect2) = scene.intersect(beam, f32::MAX, true) {
                        let rad2 = self.indirect_lighter.light(&isect2, sampler);
                    
                        let sample = IndirectSample {
                            point: isect2.point,
                            normal: isect2.facing_normal,
                            indirect_radiance: rad2 - isect2.primitive.surface.radiance
                        };

                        let q = sample.indirect_radiance.mag();
                        if let Ok(mut indirect_reservoirs) = self.indirect_reservoirs.write() {
                            indirect_reservoirs.get_mut(x, y).add_sample(sample, q, pdf, sampler);
                        }
                    }
                }
            }

            rad_emitted = isect.primitive.surface.radiance;
        } else {
            if let Ok(mut primary_hits) = self.primary_hits.write() {
                primary_hits.set(x, y, None);
            }
            rad_emitted = scene.sky_radiance;
        }

        self.add_radiance(x, y, sample, rad_emitted);
    }

    fn direct_illuminate_pixel(&self, x : usize, y : usize, sample : usize, sampler : &mut dyn Sampler) {
        let mut rad_direct = Radiance::ZERO;

        if let Ok(primary_hits) = self.primary_hits.read() {
            if let Some(primary_hit) = primary_hits.get(x, y) {
                let isect = Intersection::with_flat(primary_hit, &self.scene);

                let nrm_facing = isect.facing_normal;
                let surface = &isect.primitive.surface;
                let pnt_offset = isect.point + Vec3::from(nrm_facing) * 0.01;

                let mut res = Reservoir::<DirectSample>::default();
                let r = self.settings.radius as f32;
                for _ in 0..self.settings.candidates {
                    let mut s = sampler.get_value2();
                    s = s * r * 2.0 + Point2::new(-r, -r);
                    let sx = (s.u + x as f32).floor() as i32;
                    let sy = (s.v + y as f32).floor() as i32;
                    if sx < 0 || sy < 0 || sx >= (self.settings.width as i32) || sy >= (self.settings.height as i32) {
                        continue;
                    }

                    let res_candidate = 
                    if let Ok(direct_reservoirs) = self.direct_reservoirs.read() {
                        direct_reservoirs.get(sx as usize, sy as usize)
                    } else {
                        Default::default()
                    };

                    if res_candidate.q == 0.0 {
                        continue;
                    }

                    let mut dir_in = res_candidate.sample.point - pnt_offset;
                    let d = dir_in.mag();
                    dir_in = dir_in / d;
                    let dot = dir_in * nrm_facing;
                    let dot2 = (dir_in * res_candidate.sample.normal).abs();

                    let q = if dot > 0.0 {
                        let irad = res_candidate.sample.radiance * dot2 * dot / (d * d);
                        let rad = irad * surface.reflected(&isect, dir_in);
                        rad.mag()
                    } else {
                        0.0
                    };

                    res.add_reservoir(res_candidate, q, 1.0, sampler);
                }

                if res.w > 0.0 {
                    let mut dir_in = res.sample.point - pnt_offset;
                    let d = dir_in.mag();
                    dir_in = dir_in / d;
                    let dot = dir_in * nrm_facing;
                    let dot2 = (dir_in * res.sample.normal).abs();

                    if dot > 0.0 {
                        let ray = Ray::new(pnt_offset, dir_in);
                        let beam = Beam::new(ray, Bivec3::ZERO, Bivec3::ZERO);
                        if let Some(isect2) = self.scene.intersect(beam, f32::MAX, true) {
                            if isect2.primitive_idx == res.sample.primitive_idx {
                                let irad = res.sample.radiance * dot2 * dot / (d * d);
                                let rad = irad * surface.reflected(&isect, dir_in);
                                rad_direct = rad * res.w;
                            }
                        }                        
                    }
                }
            }
        }

        self.add_radiance(x, y, sample, rad_direct);
    }
    
    fn indirect_illuminate_pixel(&self, x : usize, y : usize, sample : usize, sampler : &mut dyn Sampler, indirect_samples: &mut [Reservoir<IndirectSample>]) {
        let mut rad_indirect = Radiance::ZERO;
        let n = self.settings.indirect_samples;

        if let Ok(primary_hits) = self.primary_hits.read() {
            if let Some(primary_hit) = primary_hits.get(x, y) {
                let isect = Intersection::with_flat(primary_hit, &self.scene);

                let nrm_facing = isect.facing_normal;
                let surface = &isect.primitive.surface;

                for sample in indirect_samples.iter_mut() {
                    *sample = Default::default();
                }
                
                let r = self.settings.radius as f32;
                for _ in 0..self.settings.candidates {
                    let mut s = sampler.get_value2();
                    s = s * r * 2.0 + Point2::new(-r, -r);
                    let sx = (s.u + x as f32).floor() as i32;
                    let sy = (s.v + y as f32).floor() as i32;
                    if sx < 0 || sy < 0 || sx >= (self.settings.width as i32) || sy >= (self.settings.height as i32) {
                        continue;
                    }

                    if let Ok(primary_hits) = self.primary_hits.read() {
                        if let Some(flat_isect_s) = primary_hits.get(sx as usize, sy as usize) {
                            let isect_s = Intersection::with_flat(flat_isect_s, &self.scene);

                            let res_candidate = 
                            if let Ok(indirect_reservoirs) = self.indirect_reservoirs.read() {
                                indirect_reservoirs.get(sx as usize, sy as usize)
                            } else {
                                Default::default()
                            };

                            if res_candidate.q == 0.0 {
                                continue;
                            }

                            for i in 0..n {
                                let r = res_candidate.sample.point - isect.point;
                                let q = res_candidate.sample.point - isect_s.point;
                                let n = res_candidate.sample.normal;
                                let j = ((n * r) * q.mag2() / ((n * q) * r.mag2())).abs();
                                indirect_samples[i].add_reservoir(res_candidate, res_candidate.q, j, sampler);
                            }
                        }
                    }
                }

                for i in 0..n {
                    if indirect_samples[i].w > 0.0 {
                        let mut dir_in = indirect_samples[i].sample.point - isect.point;
                        let d = dir_in.mag();
                        dir_in = dir_in / d;
                        let dot = dir_in * nrm_facing;

                        if dot > 0.0 {
                            let reflected = surface.reflected(&isect, dir_in);
                            rad_indirect += indirect_samples[i].sample.indirect_radiance * dot * reflected * indirect_samples[i].w;
                        }
                    }
                }
            }
        }
        
        self.add_radiance(x, y, sample, rad_indirect / (n as f32));
    }

    fn add_radiance(&self, x: usize, y: usize, sample: usize, radiance: Radiance) {
        if let Ok(mut total_radiance) = self.total_radiance.lock() {
            let rad_total = total_radiance.get(x, y) + radiance;
            total_radiance.set(x, y, rad_total);

            let color = Framebuffer::tone_map(rad_total / ((sample + 1) as f32));
            if let Ok(mut framebuffer) = self.framebuffer.lock() {
                framebuffer.set_pixel(x, y, color);
            }
        }
    }
}

struct ThreadLocal {
    sampler: Halton,
    indirect_samples: Vec<Reservoir<IndirectSample>>
}

pub struct ReSTIR {
    inner: Arc<Inner>,
}

impl ReSTIR {
    pub fn new(
        scene: Arc<Scene>,
        settings: ReSTIRSettings
    ) -> ReSTIR {
        let framebuffer = Mutex::new(Framebuffer::new(settings.width, settings.height));
        let total_radiance = Mutex::new(Raster::new(settings.width, settings.height));
        let indirect_lighter = Box::new(UniPath::new());
        let primary_hits = RwLock::new(Raster::new(settings.width, settings.height));

        let executor = Executor::new();
        let start_time = Mutex::new(Instant::now());
        let done_listener = Mutex::new(None);
        let current_sample = Mutex::new(0);

        let direct_reservoirs = RwLock::new(Raster::new(settings.width, settings.height));
        let indirect_reservoirs = RwLock::new(Raster::new(settings.width, settings.height));

        let inner = Arc::new(Inner {
            framebuffer,
            scene,
            settings,
            total_radiance,
            indirect_lighter,
            primary_hits,
            executor,
            done_listener,
            start_time,
            current_sample,
            direct_reservoirs,
            indirect_reservoirs
        });

        return ReSTIR {inner};
    }
}

impl Renderer for ReSTIR {
    fn start(&self, done: Box<DoneFunc>)
    {
        if let Ok(mut done_listener) = self.inner.done_listener.lock() {
            done_listener.replace(done);
        }

        if let Ok(mut start_time) = self.inner.start_time.lock() {
            *start_time = Instant::now();
        }

        self.inner.start_initial_sample_job();
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