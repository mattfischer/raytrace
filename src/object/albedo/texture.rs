use crate::geo;
use crate::object;

use geo::Bivec2;
use geo::Point2;

use object::Color;

pub struct Texture {
    texture: object::Texture<3>,
}

impl Texture {
    pub fn new(mut texture: object::Texture<3>) -> Texture {
        texture.generate_mipmaps();

        return Texture { texture };
    }
}

impl object::Albedo for Texture {
    fn color(&self, surface_point: Point2, surface_projection: Bivec2) -> Color {
        let values = self.texture.sample(surface_point, surface_projection);
        return Color::new(values[0], values[1], values[2]);
    }
}
