use crate::object;

use object::Intersection;
use object::Radiance;
use object::Sampler;

pub trait Lighter: Send + Sync {
    fn light(&self, isect: &Intersection, sampler: &mut dyn Sampler) -> Radiance;
}
