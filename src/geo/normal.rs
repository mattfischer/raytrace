use crate::geo;

use geo::Transformation;
use geo::Vec3;

#[derive(PartialEq, Clone, Copy)]
pub struct Normal3 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

impl Normal3 {
    pub const ZERO: Self = Self::new(0.0, 0.0, 0.0);

    pub const fn new(x: f32, y: f32, z: f32) -> Self {
        Self { x, y, z }
    }

    pub fn transform(self, xform: Transformation) -> Self {
        self * xform.inverse_matrix
    }

    pub fn inverse_transform(self, xform: Transformation) -> Self {
        self * xform.matrix
    }

    pub fn mag2(self) -> f32 {
        self * self
    }

    pub fn mag(self) -> f32 {
        self.mag2().sqrt()
    }

    pub fn normalize(self) -> Self {
        self / self.mag2().sqrt()
    }
}

impl std::default::Default for Normal3 {
    fn default() -> Self {
        Self::ZERO
    }
}

impl std::convert::From<Vec3> for Normal3 {
    fn from(value: Vec3) -> Self {
        Self::new(value.x, value.y, value.z)
    }
}

impl std::convert::From<Normal3> for Vec3 {
    fn from(value: Normal3) -> Self {
        Self::new(value.x, value.y, value.z)
    }
}

impl std::ops::Add for Normal3 {
    type Output = Self;

    fn add(self, rhs: Self) -> Self {
        Self::new(self.x + rhs.x, self.y + rhs.y, self.z + rhs.z)
    }
}

impl std::ops::AddAssign for Normal3 {
    fn add_assign(&mut self, rhs: Self) {
        *self = Self::new(self.x + rhs.x, self.y + rhs.y, self.z + rhs.z);
    }
}

impl std::ops::Sub for Normal3 {
    type Output = Self;

    fn sub(self, rhs: Self) -> Self {
        Self::new(self.x - rhs.x, self.y - rhs.y, self.z - rhs.z)
    }
}

impl std::ops::SubAssign for Normal3 {
    fn sub_assign(&mut self, rhs: Self) {
        *self = Self::new(self.x - rhs.x, self.y - rhs.y, self.z - rhs.z);
    }
}

impl std::ops::Mul for Normal3 {
    type Output = f32;

    fn mul(self, rhs: Self) -> f32 {
        self.x * rhs.x + self.y * rhs.y + self.z * rhs.z
    }
}

impl std::ops::Mul<Vec3> for Normal3 {
    type Output = f32;

    fn mul(self, rhs: Vec3) -> f32 {
        self * Self::from(rhs)
    }
}

impl std::ops::Mul<Normal3> for Vec3 {
    type Output = f32;

    fn mul(self, rhs: Normal3) -> f32 {
        Normal3::from(self) * rhs
    }
}

impl std::ops::Mul<f32> for Normal3 {
    type Output = Self;

    fn mul(self, rhs: f32) -> Self {
        Self::new(self.x * rhs, self.y * rhs, self.z * rhs)
    }
}

impl std::ops::MulAssign<f32> for Normal3 {
    fn mul_assign(&mut self, rhs: f32) {
        *self = Self::new(self.x * rhs, self.y * rhs, self.z * rhs);
    }
}

impl std::ops::Mul<Normal3> for f32 {
    type Output = Normal3;

    fn mul(self, rhs: Normal3) -> Normal3 {
        Normal3::new(self * rhs.x, self * rhs.y, self * rhs.z)
    }
}

impl std::ops::Div<f32> for Normal3 {
    type Output = Self;

    fn div(self, rhs: f32) -> Self {
        Self::new(self.x / rhs, self.y / rhs, self.z / rhs)
    }
}

impl std::ops::DivAssign<f32> for Normal3 {
    fn div_assign(&mut self, rhs: f32) {
        *self = Self::new(self.x / rhs, self.y / rhs, self.z / rhs);
    }
}

impl std::ops::Rem for Normal3 {
    type Output = Self;

    fn rem(self, rhs: Self) -> Self {
        Self::new(
            self.y * rhs.z - self.z * rhs.y,
            self.z * rhs.x - self.x * rhs.z,
            self.x * rhs.y - self.y * rhs.x,
        )
    }
}

impl std::ops::Neg for Normal3 {
    type Output = Self;

    fn neg(self) -> Self {
        Self::new(-self.x, -self.y, -self.z)
    }
}
