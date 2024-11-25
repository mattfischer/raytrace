use crate::object;

use object::Intersection;
use object::Radiance;
use object::Sampler;

pub trait Lighter {
    fn light(isect : &Intersection, sampler : &mut dyn Sampler) -> Radiance;      
}