mod point;
pub use point::Point;

mod shape;
pub use shape::Shape;

use crate::geo;
use geo::Point3;

use crate::object;
use object::Intersection;
use object::Pdf;
use object::Radiance;
use object::Sampler;

pub trait Light: Send + Sync {
    fn sample(&self, sampler: &mut dyn Sampler, pnt: Point3) -> Option<(Radiance, Point3, Pdf)>;

    fn did_intersect(&self, isect: &Intersection) -> bool;
}