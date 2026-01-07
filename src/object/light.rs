mod point;
pub use point::Point;

mod shape;
pub use shape::Shape;

use crate::geo;
use geo::Point3;
use geo::Vec3;

use crate::object;
use object::Intersection;
use object::Pdf;
use object::Radiance;
use object::Sampler;
use object::Scene;

pub struct LightSample {
    pub radiance: Radiance,
    pub origin: Point3,
    pub direction: Vec3,
    pub pdf: Pdf,
    pub distance: f32
}

pub trait Light: Send + Sync {
    fn sample(&self, sampler: &mut dyn Sampler, pnt: Point3) -> Option<LightSample>;
    fn pdf(&self, isect: &Intersection) -> Pdf;
    fn radiance(&self, isect: &Intersection) -> Radiance;

    fn test_visible(&self, scene: &Scene, sample: &LightSample) -> bool;
}