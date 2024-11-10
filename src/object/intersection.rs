use crate::geo;
use crate::object;

use object::Primitive;
use object::ShapeIntersection;
use object::Scene;
use geo::Beam;

pub struct Intersection<'a, 'b> {
    scene : &'a Scene,
    primitive : Option<&'a Primitive>,
    shape_isect : ShapeIntersection,
    beam : &'b Beam
}

impl<'a, 'b> Intersection<'a, 'b> {
    pub fn new(scene : &'a Scene, primitive : Option<&'a Primitive>, beam : &'b Beam, shape_isect : ShapeIntersection) -> Intersection<'a, 'b> {
        Intersection{scene, primitive, shape_isect, beam}
    }
}