use crate::geo;
use crate::object;

use geo::Vec3;

use object::Albedo;
use object::Brdf;
use object::Color;
use object::Intersection;
use object::NormalMap;
use object::Radiance;
use object::Sampler;

pub struct Surface {
    pub albedo: Box<dyn Albedo>,
    pub brdfs: Vec<Box<dyn Brdf>>,
    pub transmit_ior: f32,
    pub radiance: Radiance,
    pub normal_map: Option<NormalMap>,
    pub opaque: bool,
}

impl Surface {
    pub fn new(
        albedo: Box<dyn Albedo>,
        brdfs: Vec<Box<dyn Brdf>>,
        transmit_ior: f32,
        radiance: Radiance,
        normal_map: Option<NormalMap>,
    ) -> Surface {
        let mut opaque = false;
        for brdf in brdfs.iter() {
            if brdf.opaque() {
                opaque = true;
            }
        }

        Surface {
            albedo,
            brdfs,
            transmit_ior,
            radiance,
            normal_map,
            opaque,
        }
    }

    pub fn reflected(&self, isect: &Intersection, dir_in: Vec3) -> Color {
        let mut color = Color::ZERO;
        let mut color_transmit = Color::ONE;

        for brdf in self.brdfs.iter() {
            color += color_transmit
                * brdf.reflected(
                    dir_in,
                    isect.facing_normal,
                    -isect.ray.direction,
                    isect.albedo,
                );
            color_transmit =
                color_transmit * brdf.transmitted(dir_in, isect.facing_normal, isect.albedo);
        }

        return color;
    }

    pub fn transmitted(&self, isect: &Intersection, dir_in: Vec3) -> Color {
        let mut color_transmit = Color::ONE;

        for brdf in self.brdfs.iter() {
            color_transmit =
                color_transmit * brdf.transmitted(dir_in, -isect.facing_normal, isect.albedo);
        }

        return color_transmit;
    }

    pub fn sample(
        &self,
        isect: &Intersection,
        sampler: &mut dyn Sampler,
    ) -> (Color, Vec3, Option<f32>) {
        let dir_out = -isect.ray.direction;
        let nrm_facing = isect.facing_normal;

        let mut transmit_threshold = 0.0;
        if !self.opaque {
            let reverse = isect.normal * dir_out < 0.0;

            let mut ratio = 1.0 / self.transmit_ior;
            if reverse {
                ratio = 1.0 / ratio;
            }

            let c1 = dir_out * nrm_facing;
            let c2 = (1.0 - ratio.powi(2) * (1.0 - c1.powi(2))).sqrt();

            let dir_in = Vec3::from(nrm_facing) * (ratio * c1 - c2) - dir_out * ratio;
            let throughput = self.transmitted(isect, -dir_out);
            transmit_threshold = throughput.max_component().min(1.0);
            let roulette = sampler.get_value();

            if roulette < transmit_threshold {
                let color =
                    self.transmitted(isect, dir_in) / (dir_out * nrm_facing * transmit_threshold);
                return (color, dir_in, None);
            }
        }

        let mut idx = 0;
        if self.brdfs.len() > 1 {
            let sample = sampler.get_value();
            idx = (((self.brdfs.len() as f32) * sample).floor() as usize).min(self.brdfs.len() - 1);
        }
        let brdf = &self.brdfs[idx];

        let dir_in = brdf.sample(sampler, nrm_facing, dir_out);
        let pdf = self.pdf(isect, dir_in);
        let color = self.reflected(isect, dir_in) / (1.0 - transmit_threshold);

        return (color, dir_in, Some(pdf));
    }

    pub fn pdf(&self, isect: &Intersection, dir_in: Vec3) -> f32 {
        let dir_out = -isect.ray.direction;
        let nrm_facing = isect.facing_normal;

        let mut total_pdf = 0.0;
        for brdf in self.brdfs.iter() {
            total_pdf += brdf.pdf(dir_in, nrm_facing, dir_out);
        }
        total_pdf /= self.brdfs.len() as f32;

        return total_pdf;
    }
}
