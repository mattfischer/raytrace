use crate::object;
use crate::geo;

use geo::Vec3;
use geo::Normal3;
use object::Color;

use std::f32::consts::PI;

#[derive(Debug)]
pub struct Lambert {
    strength : f32
}

impl Lambert {
    pub fn new(strength : f32) -> Lambert {
        Lambert {strength}
    }
}

impl object::Brdf for Lambert {
    fn reflected(&self, _dir_in : Vec3, _nrm : Normal3, _dir_out : Vec3, albedo : Color) -> Color {
        return albedo * self.strength / PI;
    }

    fn transmitted(&self, _dir_in : Vec3, _nrm : Normal3, _albedo : Color) -> Color {
        return Color::ZERO;
    }

    fn lambert(&self) -> f32 {
        return self.strength;
    }
}