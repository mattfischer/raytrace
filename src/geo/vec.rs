use crate::geo;

use geo::Transformation;

#[derive(PartialEq, Clone, Copy)]
pub struct Vec3 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

impl Vec3 {
    pub const ZERO: Self = Self::new(0.0, 0.0, 0.0);

    pub const fn new(x: f32, y: f32, z: f32) -> Self {
        Vec3 { x, y, z }
    }

    pub fn with_spherical(phi: f32, theta: f32, r: f32) -> Self {
        Vec3::new(
            r * phi.cos() * theta.cos(),
            r * phi.sin() * theta.cos(),
            r * theta.sin(),
        )
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

    pub fn transform(self, xform: Transformation) -> Self {
        xform.matrix * self
    }

    pub fn inverse_transform(self, xform: Transformation) -> Self {
        xform.inverse_matrix * self
    }
}

impl std::default::Default for Vec3 {
    fn default() -> Self {
        Self::ZERO
    }
}

impl std::ops::Add for Vec3 {
    type Output = Self;

    fn add(self, rhs: Self) -> Self {
        Self::new(self.x + rhs.x, self.y + rhs.y, self.z + rhs.z)
    }
}

impl std::ops::AddAssign for Vec3 {
    fn add_assign(&mut self, rhs: Self) {
        *self = Self::new(self.x + rhs.x, self.y + rhs.y, self.z + rhs.z);
    }
}

impl std::ops::Sub for Vec3 {
    type Output = Self;

    fn sub(self, rhs: Self) -> Self {
        Self::new(self.x - rhs.x, self.y - rhs.y, self.z - rhs.z)
    }
}

impl std::ops::SubAssign for Vec3 {
    fn sub_assign(&mut self, rhs: Self) {
        *self = Self::new(self.x - rhs.x, self.y - rhs.y, self.z - rhs.z);
    }
}

impl std::ops::Mul for Vec3 {
    type Output = f32;

    fn mul(self, rhs: Self) -> f32 {
        self.x * rhs.x + self.y * rhs.y + self.z * rhs.z
    }
}

impl std::ops::Mul<f32> for Vec3 {
    type Output = Self;

    fn mul(self, rhs: f32) -> Self {
        Self::new(self.x * rhs, self.y * rhs, self.z * rhs)
    }
}

impl std::ops::MulAssign<f32> for Vec3 {
    fn mul_assign(&mut self, rhs: f32) {
        *self = Self::new(self.x * rhs, self.y * rhs, self.z * rhs);
    }
}

impl std::ops::Mul<Vec3> for f32 {
    type Output = Vec3;

    fn mul(self, rhs: Vec3) -> Vec3 {
        Vec3::new(self * rhs.x, self * rhs.y, self * rhs.z)
    }
}

impl std::ops::Div<f32> for Vec3 {
    type Output = Self;

    fn div(self, rhs: f32) -> Self {
        Self::new(self.x / rhs, self.y / rhs, self.z / rhs)
    }
}

impl std::ops::DivAssign<f32> for Vec3 {
    fn div_assign(&mut self, rhs: f32) {
        *self = Self::new(self.x / rhs, self.y / rhs, self.z / rhs);
    }
}

impl std::ops::Rem for Vec3 {
    type Output = Self;
    fn rem(self, rhs: Vec3) -> Self {
        Self::new(
            self.y * rhs.z - self.z * rhs.y,
            self.z * rhs.x - self.x * rhs.z,
            self.x * rhs.y - self.y * rhs.x,
        )
    }
}

impl std::ops::Neg for Vec3 {
    type Output = Self;
    fn neg(self) -> Self {
        Self::new(-self.x, -self.y, -self.z)
    }
}

#[derive(PartialEq, Clone, Copy)]
pub struct Vec2 {
    pub u: f32,
    pub v: f32,
}

impl Vec2 {
    pub const ZERO: Vec2 = Self::new(0.0, 0.0);

    pub const fn new(u: f32, v: f32) -> Vec2 {
        Self { u, v }
    }

    pub fn mag2(self) -> f32 {
        self * self
    }

    pub fn normalize(self) -> Vec2 {
        self / self.mag2().sqrt()
    }
}

impl std::default::Default for Vec2 {
    fn default() -> Self {
        Self::ZERO
    }
}

impl std::ops::Add for Vec2 {
    type Output = Self;

    fn add(self, rhs: Self) -> Self {
        Self::new(self.u + rhs.u, self.v + rhs.v)
    }
}

impl std::ops::AddAssign for Vec2 {
    fn add_assign(&mut self, rhs: Self) {
        *self = Self::new(self.u + rhs.u, self.v + rhs.v);
    }
}

impl std::ops::Sub for Vec2 {
    type Output = Self;

    fn sub(self, rhs: Self) -> Self {
        Self::new(self.u - rhs.u, self.v - rhs.v)
    }
}

impl std::ops::SubAssign for Vec2 {
    fn sub_assign(&mut self, rhs: Self) {
        *self = Self::new(self.u - rhs.u, self.v - rhs.v);
    }
}

impl std::ops::Mul for Vec2 {
    type Output = f32;

    fn mul(self, rhs: Self) -> f32 {
        self.u * rhs.u + self.v * rhs.v
    }
}

impl std::ops::Mul<f32> for Vec2 {
    type Output = Self;

    fn mul(self, rhs: f32) -> Self {
        Self::new(self.u * rhs, self.v * rhs)
    }
}

impl std::ops::MulAssign<f32> for Vec2 {
    fn mul_assign(&mut self, f: f32) {
        *self = Self::new(self.u * f, self.v * f);
    }
}

impl std::ops::Mul<Vec2> for f32 {
    type Output = Vec2;

    fn mul(self, rhs: Vec2) -> Vec2 {
        Vec2::new(self * rhs.u, self * rhs.v)
    }
}

impl std::ops::Div<f32> for Vec2 {
    type Output = Self;

    fn div(self, rhs: f32) -> Self {
        Self::new(self.u / rhs, self.v / rhs)
    }
}

impl std::ops::DivAssign<f32> for Vec2 {
    fn div_assign(&mut self, rhs: f32) {
        *self = Self::new(self.u / rhs, self.v / rhs);
    }
}

impl std::ops::Rem for Vec2 {
    type Output = f32;
    fn rem(self, rhs: Self) -> f32 {
        self.u * rhs.v - rhs.u * self.v
    }
}

impl std::ops::Neg for Vec2 {
    type Output = Self;
    fn neg(self) -> Self {
        Self::new(-self.u, -self.v)
    }
}
