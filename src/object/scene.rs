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

    pub fn intersect<'a, 'b>(&'a self, beam : &'b Beam, max_distance : f32, closest : bool) -> Intersection<'a, 'b> {
        let mut shape_isect = ShapeIntersection::new();
        let mut primitive : Option<&Primitive> = None;

        shape_isect.distance = max_distance;
        for p in self.primitives.iter() {
            if p.shape.intersect(&beam.ray, &mut shape_isect, closest) {
                primitive = Some(&p);
            }
        }

        return Intersection::new(self, primitive, beam, shape_isect);
    }
}