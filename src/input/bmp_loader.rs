use crate::object;
use object::Texture;

use std::fs::File;
use std::io::Read;
use std::io::Seek;
use std::io::SeekFrom;

use packbytes::FromBytes;

pub struct BmpLoader;

#[derive(FromBytes)]
struct BITMAPFILEHEADER {
    _bf_type: u16,
    _bf_size: u32,
    _bf_reserved1: u16,
    _bf_reserved2: u16,
    bf_off_bits: u32
}

#[derive(FromBytes)]
struct BITMAPINFOHEADER {
  _bi_size: u32,
  bi_width: u32,
  bi_height: u32,
  _bi_planes: u16,
  _bi_bit_count: u16,
  _bi_compression: u32,
  _bi_size_image: u32,
  _bi_x_pels_per_meter: u32,
  _bi_y_pels_per_meter: u32,
  _bi_clr_used: u32,
  _bi_clr_important: u32
}

impl BmpLoader {
    pub fn load(filename: &str) -> std::io::Result<Texture<3>> {
        let mut f = File::open(filename)?;
        let bmfh = BITMAPFILEHEADER::read_packed(&mut f)?;
        let bmih = BITMAPINFOHEADER::read_packed(&mut f)?;

        f.seek(SeekFrom::Start(bmfh.bf_off_bits as u64))?;
        let width = bmih.bi_width as usize;
        let height = bmih.bi_height as usize;

        let size = width * height * 3;
        let mut bytes = vec![0 as u8; size];
        f.read_exact(bytes.as_mut_slice())?;

        let mut values = vec![0.0 as f32; size];
        for i in 0..width*height {
            values[i*3 + 0] = bytes[i*3 + 2] as f32 / 255.0;
            values[i*3 + 1] = bytes[i*3 + 1] as f32 / 255.0;
            values[i*3 + 2] = bytes[i*3 + 0] as f32 / 255.0;
        }

        return Ok(Texture::new(width, height, values));
    }
}