use crate::object;
use crate::geo;

use geo::Vec3;
use geo::Normal3;
use geo::OrthonormalBasis;

use object::Color;
use object::Sampler;

use std::f32::consts::PI;

pub struct OrenNayar {
    strength : f32,
    roughness : f32
}

impl OrenNayar {
    pub fn new(strength : f32, roughness : f32) -> OrenNayar {
        OrenNayar {strength, roughness}
    }
}

impl object::Brdf for OrenNayar {
    fn reflected(&self, dir_in : Vec3, nrm : Normal3, dir_out : Vec3, albedo : Color) -> Color {
        let cos_theta_i = dir_in * nrm;
        let sin_theta_i = (1.0 - cos_theta_i * cos_theta_i).max(0.0).sqrt();
        let tan_theta_i = sin_theta_i / cos_theta_i;

        let cos_theta_r = dir_out * nrm;
        let sin_theta_r = (1.0 - cos_theta_r * cos_theta_r).max(0.0).sqrt();
        let tan_theta_r = sin_theta_r / cos_theta_r;

        let cos_phi = if sin_theta_i < 0.001 || sin_theta_r < 0.001 {
            1.0
        } else {
            let projected_incident = (dir_in - Vec3::from(nrm) * (dir_in * nrm)) / sin_theta_i;
            let projected_outgoing = (dir_out - Vec3::from(nrm) * (dir_out * nrm)) / sin_theta_r;
            projected_incident * projected_outgoing
        };

        let sigma2 = self.roughness * self.roughness;

        let a = 1.0 - 0.5 * sigma2 / (sigma2 + 0.33);
        let b = 0.45 * sigma2 / (sigma2 + 0.09);
        
        let sin_alpha = sin_theta_i.max(sin_theta_r);
        let tan_beta = tan_theta_i.min(tan_theta_r);

        return albedo * self.strength * (a + b * cos_phi.max(0.0) * sin_alpha * tan_beta) / PI;
    }

    fn transmitted(&self, _dir_in : Vec3, _normal : Normal3, _albedo : Color) -> Color {
        return Color::ZERO;
    }

    fn lambert(&self) -> f32 {
        return self.strength;
    }

    fn sample(&self, sampler : &mut dyn Sampler, nrm : Normal3, _dir_out : Vec3) -> Vec3
    {
        let sample_point = sampler.get_value2();
        let basis = OrthonormalBasis::new(nrm.into());

        let phi = 2.0 * PI  * sample_point.u;
        let theta = f32::asin(sample_point.v.sqrt());

        let dir_in = basis.local_to_world(Vec3::with_spherical(phi, PI / 2.0 - theta, 1.0));
        
        return dir_in;
    }

    fn pdf(&self, dir_in : Vec3, nrm : Normal3, _dir_out : Vec3) -> f32
    {
        let cos_theta = (dir_in * nrm).max(0.0);
        let pdf = cos_theta / PI;

        return pdf;
    }
}