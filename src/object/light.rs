mod point;
pub use point::Point;

mod shape;
pub use shape::Shape;

use crate::geo;
use geo::Point3;

use crate::object;
use object::Sampler;
use object::Radiance;
use object::Intersection;

pub trait Light: Send + Sync {
    fn sample(&self, sampler: &mut dyn Sampler, pnt: Point3) -> Option<(Radiance, Point3, f32, Option<f32>)>;

    fn did_intersect(&self, isect: &Intersection) -> bool;
}