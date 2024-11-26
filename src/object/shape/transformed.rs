use crate::geo;
use crate::object;

use geo::Ray;
use geo::Transformation;

use object::BoundingVolume;
use object::Shape;
use object::ShapeIntersection;

#[derive(Debug)]
pub struct Transformed {
    shape : Box<dyn Shape>,
    xform : Transformation
}

impl Transformed {
    pub fn new(shape : Box<dyn Shape>, xform : Transformation) -> Transformed {
        return Transformed {shape, xform};
    }
}

impl Shape for Transformed {
    fn intersect(&self, ray : Ray, max_distance : f32, closest : bool) -> Option<ShapeIntersection> {
        let transformed_ray = ray.inverse_transform(self.xform);

        if let Some(shape_isect) = self.shape.intersect(transformed_ray, max_distance, closest) {
            let normal = shape_isect.normal.transform(self.xform);
            let tangent = shape_isect.tangent.transform(self.xform);
            return Some(ShapeIntersection::new(shape_isect.distance, normal, tangent, shape_isect.surface_point));
        }

        return None;
    }

    fn bounding_volume(&self, xform : Transformation) -> BoundingVolume {
        return self.shape.bounding_volume(self.xform.transform(xform));
    }
}