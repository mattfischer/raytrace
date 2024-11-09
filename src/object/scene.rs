use crate::geo;
use crate::object;

use geo::Beam;

use object::Camera;
use object::Intersection;
use object::Primitive;
use object::ShapeIntersection;

#[derive(Debug)]
pub struct Scene {
    camera : object::Camera,
    primitives : Vec<Primitive>
}

impl Scene {
    pub fn new(camera : Camera, primitives : Vec<Primitive>) -> Scene {
        Scene {camera, primitives}
    }

    pub fn intersect(beam : &Beam, max_distance : f32, closest : bool) -> Intersection {
        let shape_isect = ShapeIntersection::new();

        return Intersection::new(shape_isect);
    }
}