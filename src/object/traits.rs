use crate::geo;
use crate::geo::Transformation;
use crate::object;

use geo::Bivec2;
use geo::Bivec3;
use geo::Normal3;
use geo::Point2;
use geo::Point3;
use geo::Ray;
use geo::Vec3;

use object::BoundingVolume;
use object::Color;


pub trait Sampler : std::fmt::Debug {
    fn start_sample_with_index(&mut self, index : usize);
    fn start_sample_with_xys(&mut self, x : usize, y : usize, sample : usize);

    fn get_value(&mut self) -> f32;

    fn get_value2(&mut self) -> geo::Point2 {
        return Point2::new(self.get_value(), self.get_value());
    }
}

#[derive(Copy, Clone)]
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
    fn intersect(&self, ray : Ray, shape_isect : &mut ShapeIntersection, closest : bool) -> bool;
    fn bounding_volume(&self, xform : Transformation) -> BoundingVolume;

    fn sample(&self, _sampler : &mut dyn Sampler) -> Option<(Point3, Normal3, f32)> {
        return None;
    }

    fn sample_pdf(&self, _pnt : Point3) -> f32 {
        return 0.0;
    }
}

pub trait Albedo : std::fmt::Debug {
    fn color(&self, surface_point : Point2, surface_projection : Bivec2) -> Color;
}

pub trait Brdf : std::fmt::Debug {
    fn reflected(&self, dir_in : Vec3, nrm : Normal3, dir_out : Vec3, albedo : Color) -> Color;
    fn transmitted(&self, dir_in : Vec3, nrm : Normal3, albedo : Color) -> Color;
    fn lambert(&self) -> f32;

    fn sample(&self, sampler : &mut dyn Sampler, nrm : Normal3, dir_out : Vec3) -> Vec3;
    fn pdf(&self, dir_in : Vec3, nrm : Normal3, dir_out : Vec3) -> f32;

    fn opaque(&self) -> bool {
        return true;
    }
}
