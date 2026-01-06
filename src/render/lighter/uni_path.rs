use crate::geo;
use crate::object;
use crate::render;

use geo::Beam;
use geo::Bivec3;
use geo::Ray;
use geo::Vec3;

use object::Color;

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
        isect_base: &crate::object::Intersection,
        sampler: &mut dyn crate::object::Sampler,
    ) -> crate::object::Radiance {
        let mut isect = (*isect_base).clone();
        let scene = isect.scene;
        let mut beam;

        let mut rad = isect.primitive.surface.radiance;
        let mut throughput = Color::ONE;

        for generation in 0..10 {
            let surface = &isect.primitive.surface;
            let nrm_facing = isect.facing_normal;

            let pnt_offset = isect.point + Vec3::from(nrm_facing) * 0.01;
            for light in &scene.lights {
                if let Some((rad2, pnt_sample, dot_sample, pdf)) = light.sample(sampler, pnt_offset) {
                    let mut dir_in = pnt_sample - pnt_offset;
                    let d = dir_in.mag();
                    dir_in = dir_in / d;

                    let dot = dir_in * nrm_facing;
                    if dot > 0.0 {
                        let ray = Ray::new(pnt_offset, dir_in);
                        let beam = Beam::new(ray, Bivec3::ZERO, Bivec3::ZERO);
                        let isect2 = scene.intersect(beam, d, false);

                        if isect2.is_none() || light.did_intersect(&isect2.unwrap()) {
                            let irad = rad2 * dot / (d * d);
                            let pdf_brdf = if pdf.is_delta { 0.0 } else { surface.pdf(&isect, dir_in).as_f32() * dot_sample / (d * d) };
                            let pdf = pdf.as_f32();
                            let mis_weight = pdf * pdf / (pdf * pdf + pdf_brdf * pdf_brdf);
                            rad += irad * surface.reflected(&isect, dir_in) * throughput * mis_weight / pdf;
                        }
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
                let rad2 = isect2.primitive.surface.radiance;
                if rad2.mag() > 0.0 && !pdf.is_delta {
                    let dot2 = -isect2.facing_normal * dir_in;
                    let pdf_area = pdf.as_f32() * dot2
                        / (isect2.shape_isect.distance * isect2.shape_isect.distance);
                    let pdf_light = isect2.primitive.shape.sample_pdf(isect2.point).as_f32();
                    let mis_weight =
                        pdf_area * pdf_area / (pdf_area * pdf_area + pdf_light * pdf_light);

                    rad += rad2 * throughput * mis_weight;
                }

                isect = isect2;
            } else {
                rad += scene.sky_radiance * throughput;
                break;
            }
        }

        return rad;
    }
}
