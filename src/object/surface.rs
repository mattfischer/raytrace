use crate::geo;
use crate::object;

use geo::Vec3;

use object::Albedo;
use object::Brdf;
use object::Color;
use object::Intersection;

#[derive(Debug)]
pub struct Surface {
    pub albedo : Box<dyn Albedo>,
    pub brdfs : Vec<Box<dyn Brdf>>
}

impl Surface {
    pub fn new(albedo : Box<dyn Albedo>, brdfs : Vec<Box<dyn Brdf>>) -> Surface {
        Surface{albedo, brdfs}
    }

    pub fn reflected(&self, isect : &Intersection, dir_in : Vec3) -> Color {
        let mut color = Color::ZERO;
        let mut color_transmit = Color::ONE;

        for brdf in self.brdfs.iter() {
            color += color_transmit * brdf.reflected(dir_in, isect.facing_normal, -isect.beam.ray.direction, isect.albedo);
            color_transmit = color_transmit * brdf.transmitted(dir_in, isect.facing_normal, isect.albedo);
        }

        return color;
    }

    pub fn transmitted(&self, isect : &Intersection, dir_in : Vec3) -> Color {
        let mut color_transmit = Color::ONE;

        for brdf in self.brdfs.iter() {
            color_transmit = color_transmit * brdf.transmitted(dir_in, -isect.facing_normal, isect.albedo);
        }

        return color_transmit;
    }
}