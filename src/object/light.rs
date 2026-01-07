mod point;
pub use point::Point;

mod shape;
pub use shape::Shape;

use crate::geo;
use geo::Point3;
use geo::Vec3;

use crate::object;
use object::Pdf;
use object::Radiance;
use object::Sampler;
use object::Scene;

pub trait Light: Send + Sync {
    fn sample(&self, sampler: &mut dyn Sampler, pnt: Point3) -> Option<(Radiance, Vec3, Pdf)>;

    fn test_visible(&self, scene: &Scene, pnt: Point3, dir_in: Vec3) -> bool;
}