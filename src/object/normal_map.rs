use crate::geo;
use crate::object;

use geo::Bivec2;
use geo::Bivec3;
use geo::Normal3;
use geo::Point2;
use geo::Vec2;

use object::Texture;

pub struct NormalMap {
    texture : Texture<2>
}

impl NormalMap {
    pub fn new(texture : Texture<3>, magnitude : f32) -> NormalMap {
        let map = &texture.mipmaps[0];
        let mut values = Vec::with_capacity(map.width * map.height * 2);
        for j in 0..map.height {
            for i in 0..map.width {
                let mut s = 0.0;
                let mut su = 0.0;
                let mut sv = 0.0;
                for c in 0..3 {
                    s += map.at(i, j, c) / 3.0;
                    su += map.at(if i == map.width { 0 } else { i + 1 }, j, c) / 3.0;
                    sv += map.at(i, if j == map.height { 0 } else { j + 1 }, c) / 3.0;
                }

                values.push(-(su - s) * (map.width as f32) * magnitude);
                values.push(-(sv - s) * (map.height as f32) * magnitude);
            }
        }

        let mut ntexture = Texture::new(map.width, map.height, values);
        ntexture.generate_mipmaps();

        return NormalMap {texture: ntexture};
    }

    pub fn perturb_normal(&self, surface_point : Point2, surface_projection : Bivec2, nrm : Normal3, tangent : Bivec3) -> Normal3 {
        let value = self.texture.sample(surface_point, surface_projection);
        let offset = tangent * Vec2::new(value[0], value[1]);
        return (nrm + Normal3::from_vec3(offset)).normalize();
    }
}