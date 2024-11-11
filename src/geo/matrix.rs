use crate::geo;

use geo::Normal3;
use geo::Point3;
use geo::Vec3;

#[derive(Copy, Clone, Debug)]
pub struct Matrix4{
    values : [f32; 16],
    identity : bool
}

impl Matrix4 {
    pub fn new(values : [f32; 16]) -> Matrix4 {
        Matrix4 {values, identity: false}
    }

    pub fn at(&self, row : usize, col : usize) -> f32 {
        return self.values[row * 4 + col];
    }
}


impl std::ops::Mul for Matrix4 {
    type Output = Matrix4;

    fn mul(self, other : Matrix4) -> Matrix4 {
        if self.identity {
            return other;
        }

        if other.identity {
            return self;
        }

        let mut values = [0.0; 16];
        for col in 0..4 {
            for row in 0..4 {
                for i in 0..4 {
                    values[row * 4 + col] += self.at(row, i) * other.at(i, col);
                }
            }
        }

        Self::new(values)
    }
}

impl std::ops::Mul<Vec3> for Matrix4 {
    type Output = Vec3;

    fn mul(self, v : Vec3) -> Vec3 {
        if self.identity {
            return v;
        }

        let x = self.at(0, 0) * v.x + self.at(0, 1) * v.y + self.at(0, 2) * v.z;
        let y = self.at(1, 0) * v.x + self.at(1, 1) * v.y + self.at(1, 2) * v.z;
        let z = self.at(2, 0) * v.x + self.at(2, 1) * v.y + self.at(2, 2) * v.z;
        
        return Vec3::new(x, y, z);
    }
}

impl std::ops::Mul<Matrix4> for Normal3 {
    type Output = Normal3;

    fn mul(self, m : Matrix4) -> Normal3 {
        if m.identity {
            return self;
        }

        let x = m.at(0, 0) * self.x + m.at(1, 0) * self.y + m.at(2, 0) * self.z;
        let y = m.at(0, 1) * self.x + m.at(1, 1) * self.y + m.at(2, 1) * self.z;
        let z = m.at(0, 2) * self.x + m.at(1, 2) * self.y + m.at(2, 2) * self.z;
        
        return Normal3::new(x, y, z);
    }
}

impl std::ops::Mul<Point3> for Matrix4 {
    type Output = Point3;

    fn mul(self, p : Point3) -> Point3 {
        if self.identity {
            return p;
        }

        let x = self.at(0, 0) * p.x + self.at(0, 1) * p.y + self.at(0, 2) * p.z + self.at(0, 3);
        let y = self.at(1, 0) * p.x + self.at(1, 1) * p.y + self.at(1, 2) * p.z + self.at(1, 3);
        let z = self.at(2, 0) * p.x + self.at(2, 1) * p.y + self.at(2, 2) * p.z + self.at(2, 3);
        
        return Point3::new(x, y, z);
    }
}