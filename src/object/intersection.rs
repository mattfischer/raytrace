use crate::geo;
use crate::object;

use object::ShapeIntersection;

pub struct Intersection {
    shape_isect : ShapeIntersection
}

impl Intersection {
    pub fn new(shape_isect : ShapeIntersection) -> Intersection {
        Intersection{shape_isect}
    }
}