use crate::geo;
use crate::object;

use geo::Bivec2;
use geo::Point2;

use object::Color;

pub struct Solid {
    color: object::Color,
}

impl Solid {
    pub fn new(color: object::Color) -> Solid {
        Solid { color }
    }
}

impl object::Albedo for Solid {
    fn color(&self, _surface_point: Point2, _surface_projection: Bivec2) -> Color {
        return self.color;
    }
}
