use crate::geo;
use crate::object;

use geo::Normal3;
use geo::OrthonormalBasis;
use geo::Vec3;

use object::Color;
use object::Sampler;

use std::f32::consts::PI;

pub struct TorranceSparrow {
    strength: f32,
    roughness: f32,
    ior: f32,
}

impl TorranceSparrow {
    pub fn new(strength: f32, roughness: f32, ior: f32) -> TorranceSparrow {
        TorranceSparrow {
            strength,
            roughness,
            ior,
        }
    }
}

impl object::Brdf for TorranceSparrow {
    fn reflected(&self, dir_in: Vec3, nrm: Normal3, dir_out: Vec3, _albedo: Color) -> Color {
        let dir_half = (dir_in + dir_out).normalize();
        let alpha = (nrm * dir_half).min(1.0).acos();
        let cos_alpha = alpha.cos();
        let tan_alpha = alpha.tan();
        let m2 = self.roughness.powi(2);

        let d = (-tan_alpha.powi(2) / m2).exp() / (PI * m2 * cos_alpha.powi(4));

        let cos_theta_i = dir_in * nrm;
        let r0 = ((1.0 - self.ior) / (1.0 + self.ior)).powi(2);

        let f = r0 + (1.0 - r0) * (1.0 - cos_theta_i).powi(5);

        let hn = dir_half * nrm;
        let vh = dir_half * dir_out;
        let vn = nrm * dir_out;
        let ln = nrm * dir_in;
        let g = (2.0 * hn * vn / vh).min(2.0 * hn * ln / vh).min(1.0);

        return Color::ONE * self.strength * d * f * g / (4.0 * vn * ln);
    }

    fn transmitted(&self, dir_in: Vec3, nrm: Normal3, _albedo: Color) -> Color {
        let cos_theta_i = dir_in * nrm;
        let r0 = ((1.0 - self.ior) / (1.0 + self.ior)).powi(2);

        let f = r0 + (1.0 - r0) * (1.0 - cos_theta_i).powi(5);
        return Color::ONE * (1.0 - self.strength * f);
    }

    fn lambert(&self) -> f32 {
        return 0.0;
    }

    fn sample(&self, sampler: &mut dyn Sampler, nrm: Normal3, dir_out: Vec3) -> Vec3 {
        let sample_point = sampler.get_value2();
        let phi = 2.0 * PI * sample_point.u;
        let tan_theta = (-self.roughness.powi(2) * (1.0 - sample_point.v).ln()).sqrt();
        let theta = tan_theta.atan();

        let basis = OrthonormalBasis::new(nrm.into());

        let axis = basis.local_to_world(Vec3::with_spherical(phi, PI / 2.0 - theta, 1.0));
        let dir_in = -(dir_out - axis * (dir_out * axis * 2.0));

        return dir_in;
    }

    fn pdf(&self, dir_in: Vec3, nrm: Normal3, dir_out: Vec3) -> f32 {
        let axis = (dir_in + dir_out).normalize();

        let cos_theta = axis * nrm;
        let sin_theta = (1.0 - cos_theta.powi(2)).max(0.0).sqrt();
        let tan_theta = sin_theta / cos_theta;

        let m2 = self.roughness.powi(2);
        let mut pdf = (-tan_theta.powi(2) / m2).exp() / (PI * m2 * cos_theta.powi(4));
        pdf = pdf / (4.0 * (dir_out * axis));

        return pdf.min(1000.0);
    }

    fn opaque(&self) -> bool {
        return false;
    }
}
