use crate::object;

use object::Color;
use object::Radiance;

pub struct Framebuffer {
    pub width : usize,
    pub height : usize,
    pub bits : Vec<u8>
}

impl Framebuffer {
    pub fn new(width : usize, height : usize) -> Framebuffer {
        let mut bits = Vec::new();
        bits.resize(width * height, 0);

        return Framebuffer {width, height, bits};
    }

    pub fn set_pixel(&mut self, x : usize, y : usize, color : Color) {
        self.bits[(y*self.width + x) * 3 + 0] = (color.red * 255.0) as u8;
        self.bits[(y*self.width + x) * 3 + 1] = (color.green * 255.0) as u8;
        self.bits[(y*self.width + x) * 3 + 2] = (color.blue * 255.0) as u8;
    }

    pub fn tone_map(rad : Radiance) -> Color {
        let red = rad.red / (rad.red + 1.0);
        let green = rad.green / (rad.green + 1.0);
        let blue = rad.blue / (rad.blue + 1.0);

        return Color::new(red, green, blue);
    }
}