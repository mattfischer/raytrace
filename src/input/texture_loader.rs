use crate::object;

use object::Texture;

use image::ImageReader;
use image::DynamicImage;

pub struct TextureLoader;

impl TextureLoader {
    pub fn load(filename : String) -> Option<Texture<3>> {
        let mut rgb_image = None;

        if let Ok(reader) = ImageReader::open(filename) {
            if let Ok(buffer) = reader.decode() {
                match buffer {
                    DynamicImage::ImageRgb8(rgb) => rgb_image = Some(rgb),
                    _ => ()
                }
            }
        }

        if let Some(rgb_image) = rgb_image {
            let width = rgb_image.width();
            let height = rgb_image.height();
            let mut values = Vec::with_capacity((width * height * 3) as usize);
            for y in 0..height {
                for x in 0..width {
                    let p = rgb_image.get_pixel(x, y);
                    let [r, g, b] = p.0;
                    values.push((r as f32) / 255.0);
                    values.push((g as f32) / 255.0);
                    values.push((b as f32) / 255.0);
                }
            }

            return Some(Texture::new(width as usize, height as usize, values));
        } else {
            return None;
        }
    }
}