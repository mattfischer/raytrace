use crate::geo;
use crate::object;

use geo::Normal3;
use geo::OrthonormalBasis;
use geo::Vec3;

use object::Color;
use object::Sampler;

use std::f32::consts::PI;

pub struct Lambert {
    strength: f32,
}

impl Lambert {
    pub fn new(strength: f32) -> Lambert {
        Lambert { strength }
    }
}

impl object::Brdf for Lambert {
    fn reflected(&self, _dir_in: Vec3, _nrm: Normal3, _dir_out: Vec3, albedo: Color) -> Color {
        return albedo * self.strength / PI;
    }

    fn transmitted(&self, _dir_in: Vec3, _nrm: Normal3, _albedo: Color) -> Color {
        return Color::ZERO;
    }

    fn lambert(&self) -> f32 {
        return self.strength;
    }

    fn sample(&self, sampler: &mut dyn Sampler, nrm: Normal3, _dir_out: Vec3) -> Vec3 {
        let sample_point = sampler.get_value2();
        let basis = OrthonormalBasis::new(nrm.into());

        let phi = 2.0 * PI * sample_point.u;
        let theta = f32::asin(sample_point.v.sqrt());

        let dir_in = basis.local_to_world(Vec3::with_spherical(phi, PI / 2.0 - theta, 1.0));

        return dir_in;
    }

    fn pdf(&self, dir_in: Vec3, nrm: Normal3, _dir_out: Vec3) -> f32 {
        let cos_theta = (dir_in * nrm).max(0.0);
        let pdf = cos_theta / PI;

        return pdf;
    }
}
