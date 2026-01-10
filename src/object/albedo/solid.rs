use crate::geo;
use crate::object;

use geo::Bivec2;
use geo::Point2;

use object::Albedo;
use object::Color;

pub struct Solid {
    color: Color,
}

impl Solid {
    pub fn new(color: Color) -> Solid {
        Solid { color }
    }
}

impl Albedo for Solid {
    fn color(&self, _surface_point: Point2, _surface_projection: Bivec2) -> Color {
        return self.color;
    }
}
