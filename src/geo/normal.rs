use crate::geo;

use geo::Transformation;
use geo::Vec3;

#[derive(PartialEq, Clone, Copy, Debug)]
pub struct Normal3 {
    pub x: f32,
    pub y: f32,
    pub z: f32
}

impl Normal3 {
    pub const ZERO : Normal3 = Normal3::new(0.0, 0.0, 0.0);
  
    pub const fn new(x: f32, y: f32, z: f32) -> Normal3 {
        return Normal3 {x, y, z};
    }

    pub fn from_vec3(v : Vec3) -> Normal3 {
        Self::new(v.x, v.y, v.z)
    }

    pub fn to_vec3(&self) -> Vec3 {
        return Vec3::new(self.x, self.y, self.z);
    }

    pub fn transform(self, xform : Transformation) -> Normal3 {
        return self * xform.inverse_matrix;
    }

    pub fn inverse_transform(self, xform : Transformation) -> Normal3 {
        return self * xform.matrix;
    }

    pub fn mag2(self) -> f32 {
        return self * self;
    }

    pub fn mag(self) -> f32 {
        return self.mag2().sqrt();
    }

    pub fn normalize(self) -> Normal3 {
        return self / self.mag2().sqrt();
    }
}

impl std::ops::Add for Normal3 {
    type Output = Self;

    fn add(self, other: Self) -> Self {
        return Self::new(self.x + other.x, self.y + other.y, self.z + other.z);
    }
}

impl std::ops::AddAssign for Normal3 {
    fn add_assign(&mut self, other: Self) {
        *self = Self::new(self.x + other.x, self.y + other.y, self.z + other.z);
    }
}

impl std::ops::Sub for Normal3 {
    type Output = Self;

    fn sub(self, other: Self) -> Self {
        return Self::new(self.x - other.x, self.y - other.y, self.z - other.z);
    }
}

impl std::ops::SubAssign for Normal3 {
    fn sub_assign(&mut self, other: Self) {
        *self = Self::new(self.x - other.x, self.y - other.y, self.z - other.z);
    }
}

impl std::ops::Mul for Normal3 {
    type Output = f32;

    fn mul(self, other: Self) -> f32 {
        return self.x * other.x + self.y * other.y + self.z * other.z;
    }
}

impl std::ops::Mul<Vec3> for Normal3 {
    type Output = f32;

    fn mul(self, other: Vec3) -> f32 {
        return self * Self::from_vec3(other);
    }
}

impl std::ops::Mul<Normal3> for Vec3 {
    type Output = f32;

    fn mul(self, other: Normal3) -> f32 {
        return Normal3::from_vec3(self) * other;
    }
}

impl std::ops::Mul<f32> for Normal3 {
    type Output = Self;

    fn mul(self, f: f32) -> Self {
        return Self::new(self.x * f, self.y * f, self.z * f);
    }
}

impl std::ops::MulAssign<f32> for Normal3 {
    fn mul_assign(&mut self, f: f32) {
        *self = Self::new(self.x * f, self.y * f, self.z * f);
    }
}

impl std::ops::Mul<Normal3> for f32 {
    type Output = Normal3;

    fn mul(self, v: Normal3) -> Normal3 {
        return Normal3::new(self * v.x, self * v.y, self * v.z);
    }
}

impl std::ops::Div<f32> for Normal3 {
    type Output = Self;

    fn div(self, f: f32) -> Self {
        return Self::new(self.x / f, self.y / f, self.z / f);
    }
}

impl std::ops::DivAssign<f32> for Normal3 {
    fn div_assign(&mut self, f: f32) {
        *self = Self::new(self.x / f, self.y / f, self.z / f);
    }
}

impl std::ops::Rem for Normal3 {
    type Output = Self;

    fn rem(self, other: Self) -> Self {
        return Self::new(self.y * other.z - self.z * other.y,
                         self.z * other.x - self.x * other.z,
                         self.x * other.y - self.y * other.x);
    }
}

impl std::ops::Neg for Normal3 {
    type Output = Self;

    fn neg(self) -> Self {
        return Self::new(-self.x, -self.y, -self.z);
    }
}
