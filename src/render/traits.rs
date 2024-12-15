use crate::object;

use object::Intersection;
use object::Radiance;
use object::Sampler;

pub trait Lighter: Send + Sync {
    fn light(&self, isect: &Intersection, sampler: &mut dyn Sampler) -> Radiance;
}

pub trait Renderer: Send + Sync {
    fn start(&self, done: Box<dyn FnOnce(f32) + 'static + Send + Sync>);
    fn stop(&self);
    fn running(&self) -> bool;
    fn framebuffer_ptr(&self) -> *const u8;
}