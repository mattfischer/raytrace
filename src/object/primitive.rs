use crate::object;

use object::Shape;
use object::Surface;

use std::sync::Arc;

pub struct Primitive {
    pub shape: Arc<Box<dyn Shape>>,
    pub surface: Surface,
}

impl Primitive {
    pub fn new(shape: Box<dyn Shape>, surface: Surface) -> Primitive {
        Primitive { shape: Arc::new(shape), surface }
    }
}
