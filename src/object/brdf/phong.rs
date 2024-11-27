use crate::object;
use crate::geo;

use geo::Vec3;
use geo::Normal3;
use geo::OrthonormalBasis;

use object::Color;
use object::Sampler;

use std::f32::consts::PI;

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

    fn sample(&self, sampler : &mut dyn Sampler, nrm : Normal3, dir_out : Vec3) -> Vec3
    {
        let sample_point = sampler.get_value2();
        let phi = 2.0 * PI * sample_point.u;
        let theta = sample_point.v.powf(1.0 / (self.power + 1.0)).acos();
        
        let basis = OrthonormalBasis::new(dir_out);

        let dir_reflect = basis.local_to_world(Vec3::from_polar(phi, PI / 2.0 - theta, 1.0));
        let dir_in = -(dir_reflect - nrm.to_vec3() * (dir_reflect * nrm * 2.0));
        return dir_in;
    }

    fn pdf(&self, dir_in : Vec3, nrm : Normal3, dir_out : Vec3) -> f32 {
        let mut coeff = 0.0;
        let dir_reflect = -(dir_in - nrm.to_vec3() * (dir_in * nrm * 2.0));
        let dot = dir_reflect * dir_out;
        if dot > 0.0 {
            coeff = dot.powf(self.power);
        }

        let pdf = coeff * (self.power + 1.0) / (2.0 * PI);
        return pdf.min(1000.0);
    }
}