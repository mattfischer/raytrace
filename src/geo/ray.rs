use crate::geo;

use geo::Point3;
use geo::Transformation;
use geo::Vec3;

#[derive(Copy, Clone)]
pub struct Ray {
    pub origin: Point3,
    pub direction: Vec3,
}

impl Ray {
    pub fn new(origin: Point3, direction: Vec3) -> Ray {
        return Ray { origin, direction };
    }

    pub fn transform(&self, xform: Transformation) -> Ray {
        return Ray {
            origin: self.origin.transform(xform),
            direction: self.direction.transform(xform),
        };
    }

    pub fn inverse_transform(&self, xform: Transformation) -> Ray {
        return Ray {
            origin: self.origin.inverse_transform(xform),
            direction: self.direction.inverse_transform(xform),
        };
    }
}
