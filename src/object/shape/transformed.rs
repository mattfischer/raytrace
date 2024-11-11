use crate::geo;
use crate::object;

use geo::Ray;
use geo::Transformation;

use object::Shape;
use object::ShapeIntersection;

#[derive(Debug)]
pub struct Transformed {
    shape : Box<dyn Shape>,
    xform : Transformation
}

impl Shape for Transformed {
    fn intersect(&self, ray : &Ray, shape_isect : &mut ShapeIntersection, closest : bool) -> bool {
        let transformed_ray = ray.inverse_transform(&self.xform);

        if self.shape.intersect(&transformed_ray, shape_isect, closest) {
            shape_isect.normal = shape_isect.normal.transform(&self.xform);
            return true;
        }

        return false;
    
    }
}