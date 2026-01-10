use crate::geo;
use crate::object;
use crate::render;

use geo::Beam;
use geo::Bivec3;
use geo::Ray;
use geo::Vec3;

use object::Color;
use object::Intersection;
use object::Radiance;
use object::Sampler;

use render::Lighter;

pub struct UniPath;

impl UniPath {
    pub fn new() -> UniPath {
        return UniPath {};
    }
}

impl Lighter for UniPath {
    fn light(
        &self,
        isect_base: &Intersection,
        sampler: &mut dyn Sampler,
    ) -> Radiance {
        let mut isect = (*isect_base).clone();
        let scene = isect.scene;
        let mut beam;

        let mut rad = Radiance::ZERO;
        if let Some(light) = &isect.primitive.light {
            rad += light.radiance_from_isect(isect_base);
        }
        let mut throughput = Color::ONE;

        for generation in 0..10 {
            let surface = &isect.primitive.surface;
            let nrm_facing = isect.facing_normal;

            let pnt_offset = isect.point + Vec3::from(nrm_facing) * 0.01;
            for light in &scene.lights {
                if let Some(sample) = light.sample(sampler, pnt_offset) {
                    let dot = sample.direction * nrm_facing;
                    if dot > 0.0 && light.test_visible(scene, &sample) {
                        let irad = sample.radiance * dot;
                        let pdf_brdf = if sample.pdf.is_delta { 0.0 } else { surface.pdf(&isect, sample.direction).as_f32() };
                        let pdf = sample.pdf.as_f32();
                        let mis_weight = pdf * pdf / (pdf * pdf + pdf_brdf * pdf_brdf);
                        rad += irad * surface.reflected(&isect, sample.direction) * throughput * mis_weight / pdf;
                    }
                }
            }

            let (reflected, dir_in, pdf) = surface.sample(&isect, sampler);
            let reverse = if dir_in * nrm_facing > 0.0 { 1.0 } else { -1.0 };
            let dot = dir_in * nrm_facing * reverse;

            let pnt_offset = isect.point + Vec3::from(nrm_facing) * 0.01 * reverse;
            if dot <= 0.0 {
                break;
            }

            let mut threshold = 1.0;
            let roulette = sampler.get_value();
            if generation > 0 {
                threshold = throughput.max_component().min(1.0);
            }

            if roulette >= threshold {
                break;
            }

            throughput = throughput * reflected * dot / (pdf.as_f32() * threshold);

            let reflect_ray = Ray::new(pnt_offset, dir_in);
            beam = Beam::new(reflect_ray, Bivec3::ZERO, Bivec3::ZERO);
            let isect2 = scene.intersect(beam, f32::MAX, true);

            if let Some(isect2) = isect2 {
                if let Some(light) = &isect2.primitive.light {
                    let pdf_light = if pdf.is_delta { 0.0 } else { light.pdf(&isect2).as_f32() }; 
                    let pdf = pdf.as_f32();
                    let mis_weight = pdf * pdf / (pdf * pdf + pdf_light * pdf_light);

                    let rad2 = light.radiance_from_isect(&isect2);
                    rad += rad2 * throughput * mis_weight;
                }

                isect = isect2;
            } else {
                for light in &scene.sky_lights {
                    let rad2 = light.radiance_from_direction(dir_in);
                    rad += rad2 * throughput;
                }
                break;
            }
        }

        return rad;
    }
}
