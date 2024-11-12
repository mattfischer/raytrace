use crate::geo;
use crate::geo::Transformation;
use crate::object;

use geo::Bivec2;
use geo::Bivec3;
use geo::Normal3;
use geo::Point2;
use geo::Ray;
use geo::Vec3;

use object::BoundingVolume;
use object::Color;

pub struct ShapeIntersection {
    pub distance : f32,
    pub normal : Normal3,
    pub tangent : Bivec3,
    pub surface_point : Point2
}

impl ShapeIntersection {
    pub fn new() -> ShapeIntersection {
        ShapeIntersection {distance: 0.0, normal: Normal3::ZERO, tangent: Bivec3::ZERO, surface_point: Point2::ZERO}
    }
}

pub trait Shape : std::fmt::Debug {
    fn intersect(&self, ray : &Ray, shape_isect : &mut ShapeIntersection, closest : bool) -> bool;
    fn bounding_volume(&self, xform : Transformation) -> BoundingVolume;
}

pub trait Albedo : std::fmt::Debug {
    fn color(&self, surface_point : Point2, surface_projection : Bivec2) -> Color;
}

pub trait Brdf : std::fmt::Debug {
    fn reflected(&self, dir_in : Vec3, nrm : Normal3, dir_out : Vec3, albedo : Color) -> Color;
    fn transmitted(&self, dir_in : Vec3, nrm : Normal3, albedo : Color) -> Color;
    fn lambert(&self) -> f32;
}