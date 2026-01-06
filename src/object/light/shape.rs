use crate::geo;
use geo::Point3;

use crate::object;
use object::Intersection;
use object::Light;
use object::Radiance;
use object::Sampler;

use std::sync::Arc;

pub struct Shape {
    shape: Arc<Box<dyn object::Shape>>,
    radiance: Radiance
}

impl Shape {
    pub fn new(shape: Arc<Box<dyn object::Shape>>, radiance: Radiance) -> Shape {        
        return Shape { shape, radiance };
    }
}

impl Light for Shape {
    fn sample(&self, sampler: &mut dyn Sampler, pnt: Point3) -> Option<(Radiance, Point3, f32, Option<f32>)>
    {
        if let Some((pnt_sample, nrm_sample, pdf)) = self.shape.sample(sampler) {
            let mut dir_out = pnt - pnt_sample;
            let d = dir_out.mag();
            dir_out = dir_out / d;
            let dot_sample = f32::abs(dir_out * nrm_sample);
            let rad = self.radiance * dot_sample;
            return Some((rad, pnt_sample, dot_sample, Some(pdf)));
        }
        return None;
    }

    fn did_intersect(&self, isect: &Intersection) -> bool
    {
        return Arc::ptr_eq(&isect.primitive.shape, &self.shape);
    }
  
}