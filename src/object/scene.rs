use crate::geo;
use crate::object;

use object::Camera;
use object::Primitive;

#[derive(Debug)]
pub struct Scene {
    camera : object::Camera,
    primitives : Vec<Primitive>
}

impl Scene {
    pub fn new(camera : Camera, primitives : Vec<Primitive>) -> Scene {
        Scene {camera, primitives}
    }
}