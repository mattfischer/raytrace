use crate::object;

use object::Shape;
use object::Surface;

#[derive(Debug)]
pub struct Primitive {
    pub shape : Box<dyn Shape>,
    pub surface : Surface
}

impl Primitive {
    pub fn new(shape : Box<dyn Shape>, surface : Surface) -> Primitive {
        Primitive{shape, surface}
    }
}