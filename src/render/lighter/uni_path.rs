use crate::geo;
use crate::object;
use crate::render;

use geo::Bivec3;
use geo::Ray;
use geo::Beam;

use object::Color;

use render::Lighter;

pub struct UniPath;

impl Lighter for UniPath {
    fn light(&self, isect_base : &crate::object::Intersection, sampler : &mut dyn crate::object::Sampler) -> crate::object::Radiance {
        let mut isect = (*isect_base).clone();
        let scene = isect.scene;
        let mut beam;

        let mut rad = isect.primitive.surface.radiance;
        let mut throughput = Color::ONE;

        for generation in 0..10 {
            let surface = &isect.primitive.surface;
            let nrm_facing = isect.facing_normal;
            let dir_out = -isect.beam.ray.direction;

            let pnt_offset = isect.point + nrm_facing.to_vec3() * 0.01;
            for idx in scene.area_lights.iter() {
                let light = &scene.primitives[*idx];
                let rad2 = light.surface.radiance;

                if let Some((pnt2, nrm2, pdf)) = light.shape.sample(sampler) {
                    let mut dir_in = pnt2 - pnt_offset;
                    let d = dir_in.mag();
                    dir_in = dir_in / d;
                    let dot2 = (dir_in * nrm2).abs();

                    let dot = dir_in * nrm_facing;
                    if dot <= 0.0 {
                        continue;
                    }

                    let ray = Ray::new(pnt_offset, dir_in);
                    let beam = Beam::new(ray, Bivec3::ZERO, Bivec3::ZERO);
                    let isect2 = scene.intersect(&beam, d, false);

                    if isect2.is_none() || std::ptr::eq(isect2.unwrap().primitive, light) {
                        let irad = rad2 * dot2 * dot / (d * d);
                        let pdf_brdf = surface.pdf(&isect, dir_in) * dot2 / (d * d);
                        let mis_weight = pdf * pdf / (pdf * pdf + pdf_brdf * pdf_brdf);
                        rad += irad * surface.reflected(&isect, dir_in) * throughput * mis_weight / pdf;
                    }
                }
            }

            for point_light in scene.point_lights.iter() {
                let mut dir_in = point_light.position - pnt_offset;
                let d = dir_in.mag();
                dir_in = dir_in / d;
    
                let dot = dir_in * nrm_facing;
                if dot > 0.0 {
                    let ray = Ray::new(pnt_offset, dir_in);
                    let beam = Beam::new(ray, Bivec3::ZERO, Bivec3::ZERO);
                    let isect2 = scene.intersect(&beam, d, false);
    
                    if isect2.is_none() {
                        let irad = point_light.radiance * dot / (d * d);
                        rad += irad * surface.reflected(&isect, dir_in);
                    }
                }
            }
    
            let (reflected, dir_in, pdf) = surface.sample(&isect, sampler);
            let reverse = if dir_in * nrm_facing > 0.0 { 1.0 } else { -1.0 };
            let dot = dir_in * nrm_facing * reverse;

            let pnt_offset = isect.point + nrm_facing.to_vec3() * 0.01 * reverse;
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

            throughput = throughput * reflected * dot / (pdf.unwrap_or(1.0) * threshold);

            let reflect_ray = Ray::new(pnt_offset, dir_in);
            beam = Beam::new(reflect_ray, Bivec3::ZERO, Bivec3::ZERO);
            let isect2 = scene.intersect(&beam, f32::MAX, true);

            if let Some(isect2) = isect2 {
                let rad2 = isect2.primitive.surface.radiance;
                if rad2.mag() > 0.0 && pdf.is_some() {
                    let dot2 = -isect2.facing_normal * dir_in;
                    let pdf_area = pdf.unwrap_or(1.0) * dot2 / (isect2.shape_isect.distance * isect2.shape_isect.distance);
                    let pdf_light = isect2.primitive.shape.sample_pdf(isect2.point);
                    let mis_weight = pdf_area * pdf_area / (pdf_area * pdf_area + pdf_light * pdf_light);
                    
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