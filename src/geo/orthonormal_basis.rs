use crate::geo;

use geo::Matrix4;
use geo::Vec3;

pub struct OrthonormalBasis {
    matrix: Matrix4,
}

impl OrthonormalBasis {
    pub fn new(z: Vec3) -> Self {
        let sign = z.z.signum();
        let a = -1.0 / (sign + z.z);
        let b = z.x * z.y * a;
        let x = Vec3::new(1.0 + sign * z.x.powi(2) * a, sign * b, -sign * z.x);
        let y = Vec3::new(b, sign + z.y.powi(2) * a, -z.y);

        let matrix = Matrix4::new([
            x.x, y.x, z.x, 0.0, x.y, y.y, z.y, 0.0, x.z, y.z, z.z, 0.0, 0.0, 0.0, 0.0, 1.0,
        ]);

        return Self { matrix };
    }

    pub fn world_to_local(&self, world: Vec3) -> Vec3 {
        world * self.matrix
    }

    pub fn local_to_world(&self, local: Vec3) -> Vec3 {
        self.matrix * local
    }
}
