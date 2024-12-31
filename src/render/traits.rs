use crate::object;
use crate::render;

use object::Intersection;
use object::Radiance;
use object::Sampler;
use object::Scene;

use render::ExecutorJob;
use render::Framebuffer;

use std::sync::Arc;
use std::sync::Mutex;

pub trait Lighter: Send + Sync {
    fn light(&self, isect: &Intersection, sampler: &mut dyn Sampler) -> Radiance;
    fn create_prerender_jobs(&self, scene: Arc<Scene>, framebuffer: Arc<Mutex<Framebuffer>>) -> Vec<Box<dyn ExecutorJob>> {
        return Vec::new();
    }
}

pub trait Renderer: Send + Sync {
    fn start(&self, done: Box<dyn FnOnce(f32) + 'static + Send + Sync>);
    fn stop(&self);
    fn running(&self) -> bool;
    fn framebuffer_ptr(&self) -> *const u8;
}