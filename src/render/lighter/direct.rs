use crate::geo;
use crate::object;
use crate::render;

use geo::Vec3;

use object::Radiance;

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

        let mut rad = Radiance::ZERO;
        if let Some(light) = &primitive.light {
            rad += light.radiance(isect);
        }
        let pnt_offset = isect.point + Vec3::from(nrm_facing) * 0.01;

        for light in &scene.lights {
            if let Some(sample) = light.sample(sampler, pnt_offset) {
                let dot = sample.direction * nrm_facing;

                if dot > 0.0 && light.test_visible(scene, &sample) {
                    let irad = sample.radiance * dot;
                    rad += irad * surface.reflected(isect, sample.direction) / sample.pdf.as_f32();
                }
            }
        }

        return rad;
    }
}
