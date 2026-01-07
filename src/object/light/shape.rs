use crate::geo;
use geo::Beam;
use geo::Bivec3;
use geo::Point3;
use geo::Ray;
use geo::Vec3;

use crate::object;
use object::Intersection;
use object::Light;
use object::LightSample;
use object::Pdf;
use object::Radiance;
use object::Sampler;
use object::Scene;

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
    fn sample(&self, sampler: &mut dyn Sampler, pnt: Point3) -> Option<LightSample>
    {
        if let Some((pnt_sample, nrm_sample, pdf_area)) = self.shape.sample(sampler) {
            let mut dir_in = pnt_sample - pnt;
            let d = dir_in.mag();
            dir_in = dir_in / d;
            let dot = f32::abs(dir_in * nrm_sample);
            let pdf = pdf_area.as_f32() * d * d / dot;
            return Some(LightSample { radiance: self.radiance, origin: pnt, direction: dir_in, pdf: Pdf::new(pdf, false), distance: d } );
        }
        return None;
    }

    fn pdf(&self, isect: &Intersection) -> Pdf
    {
        let dot = isect.ray.direction * isect.facing_normal;
        let d = isect.shape_isect.distance;
        return Pdf::new(self.shape.pdf(isect.point).as_f32() * d * d / dot, false);
    }

    fn radiance(&self, _isect: &Intersection) -> Radiance
    {
        return self.radiance;
    }

    fn test_visible(&self, scene: &Scene, sample: &LightSample) -> bool
    {
        let ray = Ray::new(sample.origin, sample.direction);
        let beam = Beam::new(ray, Bivec3::ZERO, Bivec3::ZERO);
        let isect = scene.intersect(beam, sample.distance, true);

        return isect.is_none() || Arc::ptr_eq(&isect.unwrap().primitive.shape, &self.shape);
    }  
}