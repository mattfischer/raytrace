use super::Shape;
use super::Surface;

#[derive(Debug)]
pub struct Primitive {
    shape : Box<dyn Shape>,
    surface : Surface
}

impl Primitive {
    pub fn new(shape : Box<dyn Shape>, surface : Surface) -> Primitive {
        Primitive{shape, surface}
    }
}