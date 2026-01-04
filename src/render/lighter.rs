mod direct;
pub use direct::Direct;

mod irradiance_cached;
pub use irradiance_cached::IrradianceCached;
pub use irradiance_cached::IrradianceCachedSettings;

mod uni_path;
pub use uni_path::UniPath;

use std::sync::Arc;
use std::sync::Mutex;

use crate::object;
use object::Intersection;
use object::Radiance;
use object::Sampler;
use object::Scene;

use crate::render;
use render::ExecutorJob;
use render::Framebuffer;

pub trait Lighter: Send + Sync {
    fn light(&self, isect: &Intersection, sampler: &mut dyn Sampler) -> Radiance;
    fn create_prerender_jobs(
        &self,
        _scene: Arc<Scene>,
        _framebuffer: Arc<Mutex<Framebuffer>>,
    ) -> Vec<Box<dyn ExecutorJob>> {
        return Vec::new();
    }
}

