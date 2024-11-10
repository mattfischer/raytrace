use crate::object;
use crate::geo;

use geo::Vec3;
use geo::Normal3;
use object::Color;

use std::f32::consts::PI;

#[derive(Debug)]
pub struct Phong {
    strength : f32,
    power : f32
}

impl Phong {
    pub fn new(strength : f32, power : f32) -> Phong {
        Phong {strength, power}
    }
}

impl object::Brdf for Phong {
    fn reflected(&self, dir_in : Vec3, nrm : Normal3, dir_out : Vec3, _albedo : Color) -> Color {
        let dir_reflect = -(dir_in - nrm.to_vec3() * (2.0 * nrm * dir_in));
        let dot = dir_reflect * dir_out;
        let coeff = if dot > 0.0 { dot.powf(self.power) } else { 0.0 };
        
        return Color::ONE * self.strength * coeff * (self.power + 1.0) / (2.0 * PI);
    }

    fn transmitted(&self, _dir_in : Vec3, _normal : Normal3, _albedo : Color) -> Color {
        return Color::ONE * (1.0 - self.strength);
    }

    fn lambert(&self) -> f32 {
        return self.strength;
    }
}