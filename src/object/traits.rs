use crate::geo;
use crate::object;

use geo::Normal3;
use geo::Ray;
use geo::Vec3;
use object::Color;

pub struct ShapeIntersection {
    pub distance : f32,
    pub normal : geo::Normal3
}

impl ShapeIntersection {
    pub fn new() -> ShapeIntersection {
        ShapeIntersection {distance: 0.0, normal: Normal3::new(0.0, 0.0, 0.0)}
    }
}

pub trait Shape : std::fmt::Debug {
    fn intersect(&self, ray : &Ray, shape_isect : &mut ShapeIntersection, closest : bool) -> bool;
}

pub trait Albedo : std::fmt::Debug {
}

pub trait Brdf : std::fmt::Debug {
    fn reflected(&self, dir_in : Vec3, nrm : Normal3, dir_out : Vec3, albedo : Color) -> Color;
    fn transmitted(&self, dir_in : Vec3, nrm : Normal3, albedo : Color) -> Color;
    fn lambert(&self) -> f32;
}