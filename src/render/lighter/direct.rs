use crate::geo;
use crate::object;
use crate::render;

use geo::Beam;
use geo::Bivec3;
use geo::Ray;
use geo::Vec3;

use render::Lighter;

pub struct Direct;

impl Direct {
    pub fn new() -> Direct {
        return Direct {};
    }
}

impl Lighter for Direct {
    fn light(
        &self,
        isect: &object::Intersection,
        sampler: &mut dyn object::Sampler,
    ) -> object::Radiance {
        let scene = isect.scene;
        let primitive = isect.primitive;
        let surface = &primitive.surface;
        let nrm_facing = isect.facing_normal;

        let mut rad = surface.radiance;
        let pnt_offset = isect.point + Vec3::from(nrm_facing) * 0.01;

        for idx in scene.area_lights.iter() {
            let light = &scene.primitives[*idx];
            let rad2 = light.surface.radiance;

            if let Some((pnt_sample, nrm_sample, pdf)) = light.shape.sample(sampler) {
                let mut dir_in = pnt_sample - pnt_offset;
                let d = dir_in.mag();
                dir_in = dir_in / d;
                let dot_sample = (dir_in * nrm_sample).abs();

                let dot = dir_in * nrm_facing;
                if dot > 0.0 {
                    let ray = Ray::new(pnt_offset, dir_in);
                    let beam = Beam::new(ray, Bivec3::ZERO, Bivec3::ZERO);
                    let isect2 = scene.intersect(beam, d, false);

                    if isect2.is_none() || std::ptr::eq(isect2.unwrap().primitive, light) {
                        let irad = rad2 * dot_sample * dot / (d * d);
                        rad += irad * surface.reflected(isect, dir_in) / pdf;
                    }
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
                let isect2 = scene.intersect(beam, d, false);

                if isect2.is_none() {
                    let irad = point_light.radiance * dot / (d * d);
                    rad += irad * surface.reflected(isect, dir_in);
                }
            }
        }

        return rad;
    }
}
