use crate::geo;

use geo::Transformation;

#[derive(Copy, Clone)]
pub struct Ray {
    pub origin : geo::Point3,
    pub direction : geo::Vec3
}

impl Ray {
    pub fn transform(&self, xform : Transformation) -> Ray {
        return Ray {origin : self.origin.transform(xform), direction: self.direction.transform(xform)};
    }

    pub fn inverse_transform(&self, xform : Transformation) -> Ray {
        return Ray {origin : self.origin.inverse_transform(xform), direction: self.direction.inverse_transform(xform)};
    }
}