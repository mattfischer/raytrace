use crate::geo;

use geo::Bivec2;
use geo::Point2;

use std::f32::consts::PI;

pub struct MipMap<const N: usize> {
    pub width: usize,
    pub height: usize,
    values: Vec<f32>,
}

impl<const N: usize> MipMap<N> {
    pub fn new(width: usize, height: usize, values: Vec<f32>) -> MipMap<N> {
        return MipMap {
            width,
            height,
            values,
        };
    }

    pub fn at(&self, x: usize, y: usize, channel: usize) -> f32 {
        return self.values[(y * self.width + x) * N + channel];
    }

    pub fn sample(&self, sample_point: Point2, weight: f32, values: &mut [f32; N]) {
        let fx = sample_point.u * (self.width as f32);
        let fy = sample_point.v * (self.height as f32);

        let x = fx.floor() as usize;
        let y = fy.floor() as usize;
        let x1 = (x + 1) % self.width;
        let y1 = (y + 1) % self.height;

        let dx = fx - (x as f32);
        let dy = fy - (y as f32);

        for c in 0..N {
            let v = self.at(x, y, c) * (1.0 - dx) * (1.0 - dy)
                + self.at(x1, y, c) * dx * (1.0 - dy)
                + self.at(x, y1, c) * (1.0 - dx) * dy
                + self.at(x1, y1, c) * dx * dy;
            values[c] += weight * v;
        }
    }
}

pub struct Texture<const N: usize> {
    pub width: usize,
    pub height: usize,
    pub mipmaps: Vec<MipMap<N>>,
}

fn next_power_of_two(x: usize) -> usize {
    let mut pot: usize;
    let mut y = x;

    let mut i = 0;
    loop {
        if y == 1 {
            pot = 1 << i;
            break;
        } else {
            y = y >> 1;
        }
        i += 1;
    }

    if pot != x {
        pot *= 2;
    }

    return pot;
}

fn resample_mipmap<const N: usize>(
    mipmap: MipMap<N>,
    new_size: usize,
    horizontal: bool,
) -> MipMap<N> {
    let (new_width, new_height, fixed_size, old_size) = if horizontal {
        (new_size, mipmap.height, mipmap.height, mipmap.width)
    } else {
        (mipmap.width, new_size, mipmap.width, mipmap.height)
    };

    let mut values = Vec::new();
    values.resize(new_width * new_height * N, 0.0);
    for j in 0..fixed_size {
        for i in 0..new_size {
            let x = if horizontal { i } else { j };
            let y = if horizontal { j } else { i };

            let si = (i * old_size) as f32 / new_size as f32;
            let a = 3;
            let af = a as f32;
            for c in 0..N {
                values[(y * new_width + x) * N + c] = 0.0;
            }

            let ceil = si.ceil() as i32;
            let floor = si.floor() as i32;
            for ii in ceil - a..=floor + a {
                let u = si - (ii as f32);
                let lanczos = if u == 0.0 {
                    1.0
                } else {
                    af * (PI * u).sin() * (PI * u / af).sin() / (PI * PI * u * u)
                };

                for c in 0..N {
                    if ii >= 0 && ii < old_size as i32 {
                        let sample = if horizontal {
                            mipmap.at(ii as usize, y, c)
                        } else {
                            mipmap.at(x, ii as usize, c)
                        };
                        values[(y * new_width + x) * N + c] += lanczos * sample;
                    }
                }
            }
        }
    }

    return MipMap {
        width: new_width,
        height: new_height,
        values,
    };
}

impl<const N: usize> Texture<N> {
    pub fn new(width: usize, height: usize, values: Vec<f32>) -> Texture<N> {
        let mut base_map = MipMap::new(width, height, values);

        let width_pot = next_power_of_two(width);
        let height_pot = next_power_of_two(height);
        if width_pot != width {
            base_map = resample_mipmap(base_map, width_pot, true);
        }

        if height_pot != height {
            base_map = resample_mipmap(base_map, height_pot, false);
        }

        let mipmaps = vec![base_map];

        return Texture {
            width,
            height,
            mipmaps,
        };
    }

    pub fn sample(&self, sample_point: Point2, sample_projection: Bivec2) -> [f32; N] {
        let lf = self.select_mipmap(sample_projection);
        let l = lf.floor() as usize;
        let level0 = &self.mipmaps[l];
        let level1 = &self.mipmaps[(l + 1).min(self.mipmaps.len() - 1)];
        let dl = lf - l as f32;

        let mut values = [0.0; N];
        level0.sample(sample_point, 1.0 - dl, &mut values);
        level1.sample(sample_point, dl, &mut values);

        return values;
    }

    pub fn generate_mipmaps(&mut self) {
        if self.mipmaps.len() > 1 {
            return;
        }

        while self.mipmaps[self.mipmaps.len() - 1].width > 1
            || self.mipmaps[self.mipmaps.len() - 1].height > 1
        {
            let last_level = &self.mipmaps[self.mipmaps.len() - 1];

            let width = (last_level.width / 2).max(1);
            let height = (last_level.height / 2).max(1);

            let mut values = Vec::with_capacity(width * height * N);
            for y in 0..height {
                for x in 0..width {
                    let (x0, x1) = (x * 2, x * 2 + 1);
                    let (y0, y1) = (y * 2, y * 2 + 1);

                    for c in 0..N {
                        let s = last_level.at(x0, y0, c)
                            + last_level.at(x1, y0, c)
                            + last_level.at(x0, y1, c)
                            + last_level.at(x1, y1, c);
                        values.push(s / 4.0);
                    }
                }
            }

            let mipmap = MipMap::new(width, height, values);
            self.mipmaps.push(mipmap);
        }
    }

    fn select_mipmap(&self, sample_projection: Bivec2) -> f32 {
        let projection_size = sample_projection
            .u
            .mag2()
            .min(sample_projection.v.mag2())
            .sqrt();

        let level = (self.mipmaps.len() - 1) as f32 + projection_size.log2();
        return level.min((self.mipmaps.len() - 1) as f32).max(0.0);
    }
}
