use crate::object;
use crate::geo;

use geo::Vec3;
use geo::Normal3;
use object::Color;

use std::f32::consts::PI;

#[derive(Debug)]
pub struct TorranceSparrow {
    strength : f32,
    roughness : f32,
    ior : f32
}

impl TorranceSparrow {
    pub fn new(strength : f32, roughness : f32, ior : f32) -> TorranceSparrow {
        TorranceSparrow {strength, roughness, ior}
    }
}

impl object::Brdf for TorranceSparrow {
    fn reflected(&self, dir_in : Vec3, nrm : Normal3, dir_out : Vec3, _albedo : Color) -> Color {
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

    fn transmitted(&self, dir_in : Vec3, nrm : Normal3, _albedo : Color) -> Color {
        let cos_theta_i = dir_in * nrm;
        let r0 = ((1.0 - self.ior) / (1.0 + self.ior)).powi(2);

        let f = r0 + (1.0 - r0) * (1.0 - cos_theta_i).powi(5);
        return Color::ONE * (1.0 - self.strength * f);
    }

    fn lambert(&self) -> f32 {
        return 0.0;
    }
}