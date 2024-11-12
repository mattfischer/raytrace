use crate::geo;

use geo::Transformation;

#[derive(PartialEq, Clone, Copy, Debug)]
pub struct Vec3 {
    pub x: f32,
    pub y: f32,
    pub z: f32
}

impl Vec3 {
    pub const ZERO : Vec3 = Vec3::new(0.0, 0.0, 0.0);
  
    pub const fn new(x: f32, y: f32, z: f32) -> Vec3 {
        return Vec3 {x, y, z};
    }

    pub fn mag2(self) -> f32 {
        return self * self;
    }

    pub fn mag(self) -> f32 {
        return self.mag2().sqrt();
    }

    pub fn normalize(self) -> Vec3 {
        return self / self.mag2().sqrt();
    }

    pub fn transform(self, xform : Transformation) -> Vec3 {
        return xform.matrix * self;
    }

    pub fn inverse_transform(self, xform : Transformation) -> Vec3 {
        return xform.inverse_matrix * self;
    }
}

impl std::ops::Add for Vec3 {
    type Output = Self;

    fn add(self, other: Self) -> Self {
        return Self::new(self.x + other.x, self.y + other.y, self.z + other.z);
    }
}

impl std::ops::AddAssign for Vec3 {
    fn add_assign(&mut self, other: Self) {
        *self = Self::new(self.x + other.x, self.y + other.y, self.z + other.z);
    }
}

impl std::ops::Sub for Vec3 {
    type Output = Self;

    fn sub(self, other: Self) -> Self {
        return Self::new(self.x - other.x, self.y - other.y, self.z - other.z);
    }
}

impl std::ops::SubAssign for Vec3 {
    fn sub_assign(&mut self, other: Self) {
        *self = Self::new(self.x - other.x, self.y - other.y, self.z - other.z);
    }
}

impl std::ops::Mul for Vec3 {
    type Output = f32;

    fn mul(self, other: Self) -> f32 {
        return self.x * other.x + self.y * other.y + self.z * other.z;
    }
}

impl std::ops::Mul<f32> for Vec3 {
    type Output = Self;

    fn mul(self, f: f32) -> Self {
        return Self::new(self.x * f, self.y * f, self.z * f);
    }
}

impl std::ops::MulAssign<f32> for Vec3 {
    fn mul_assign(&mut self, f: f32) {
        *self = Self::new(self.x * f, self.y * f, self.z * f);
    }
}

impl std::ops::Mul<Vec3> for f32 {
    type Output = Vec3;

    fn mul(self, v: Vec3) -> Vec3 {
        return Vec3::new(self * v.x, self * v.y, self * v.z);
    }
}

impl std::ops::Div<f32> for Vec3 {
    type Output = Self;

    fn div(self, f: f32) -> Self {
        return Self::new(self.x / f, self.y / f, self.z / f);
    }
}

impl std::ops::DivAssign<f32> for Vec3 {
    fn div_assign(&mut self, f: f32) {
        *self = Self::new(self.x / f, self.y / f, self.z / f);
    }
}

impl std::ops::Rem for Vec3 {
    type Output = Self;
    fn rem(self, other: Vec3) -> Self {
        return Self::new(self.y * other.z - self.z * other.y,
                         self.z * other.x - self.x * other.z,
                         self.x * other.y - self.y * other.x);
    }
}

impl std::ops::Neg for Vec3 {
    type Output = Self;
    fn neg(self) -> Self {
        return Self::new(-self.x, -self.y, -self.z);
    }
}

#[derive(PartialEq, Clone, Copy, Debug)]
pub struct Vec2 {
    pub u: f32,
    pub v: f32,
}

impl Vec2 {
    pub const ZERO : Vec2 = Vec2::new(0.0, 0.0);
  
    pub const fn new(u: f32, v: f32) -> Vec2 {
        return Vec2 {u, v};
    }

    pub fn mag2(self) -> f32 {
        return self * self;
    }

    pub fn normalize(self) -> Vec2 {
        return self / self.mag2().sqrt();
    }
}

impl std::ops::Add for Vec2 {
    type Output = Self;

    fn add(self, other: Self) -> Self {
        return Self::new(self.u + other.u, self.v + other.v);
    }
}

impl std::ops::AddAssign for Vec2 {
    fn add_assign(&mut self, other: Self) {
        *self = Self::new(self.u + other.u, self.v + other.v);
    }
}

impl std::ops::Sub for Vec2 {
    type Output = Self;

    fn sub(self, other: Self) -> Self {
        return Self::new(self.u - other.u, self.v - other.v);
    }
}

impl std::ops::SubAssign for Vec2 {
    fn sub_assign(&mut self, other: Self) {
        *self = Self::new(self.u - other.u, self.v - other.v);
    }
}

impl std::ops::Mul for Vec2 {
    type Output = f32;

    fn mul(self, other: Self) -> f32 {
        return self.u * other.u + self.v * other.v;
    }
}

impl std::ops::Mul<f32> for Vec2 {
    type Output = Self;

    fn mul(self, f: f32) -> Self {
        return Self::new(self.u * f, self.v * f);
    }
}

impl std::ops::MulAssign<f32> for Vec2 {
    fn mul_assign(&mut self, f: f32) {
        *self = Self::new(self.u * f, self.v * f);
    }
}

impl std::ops::Mul<Vec2> for f32 {
    type Output = Vec2;

    fn mul(self, v: Vec2) -> Vec2 {
        return Vec2::new(self * v.u, self * v.v);
    }
}

impl std::ops::Div<f32> for Vec2 {
    type Output = Self;

    fn div(self, f: f32) -> Self {
        return Self::new(self.u / f, self.v / f);
    }
}

impl std::ops::DivAssign<f32> for Vec2 {
    fn div_assign(&mut self, f: f32) {
        *self = Self::new(self.u / f, self.v / f);
    }
}

impl std::ops::Rem for Vec2 {
    type Output = f32;
    fn rem(self, other: Self) -> f32 {
        return self.u * other.v - other.u * self.v;
    }
}

impl std::ops::Neg for Vec2 {
    type Output = Self;
    fn neg(self) -> Self {
        return Self::new(-self.u, -self.v);
    }
}
