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

impl Shape for Transformed {
    fn intersect(&self, ray : Ray, shape_isect : &mut ShapeIntersection, closest : bool) -> bool {
        let transformed_ray = ray.inverse_transform(self.xform);

        if self.shape.intersect(transformed_ray, shape_isect, closest) {
            shape_isect.normal = shape_isect.normal.transform(self.xform);
            shape_isect.tangent = shape_isect.tangent.transform(self.xform);
            return true;
        }

        return false;
    }

    fn bounding_volume(&self, xform : Transformation) -> BoundingVolume {
        return self.shape.bounding_volume(self.xform.transform(xform));
    }
}