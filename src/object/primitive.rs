use crate::object;

use object::Light;
use object::Shape;
use object::Surface;

use std::sync::Arc;

pub struct Primitive {
    pub shape: Arc<Box<dyn Shape>>,
    pub surface: Surface,
    pub light: Option<Arc<Box<dyn Light>>>
}

impl Primitive {
    pub fn new(shape: Box<dyn Shape>, surface: Surface) -> Primitive {
        let shape = Arc::new(shape);

        let light: Option<Arc<Box<dyn Light>>>;
        if surface.radiance.mag() > 0.0 {
            light = Some(Arc::new(Box::new(object::light::Shape::new(shape.clone(), surface.radiance))));
        } else {
            light = None;
        }

        Primitive { shape, surface, light }
    }
}
