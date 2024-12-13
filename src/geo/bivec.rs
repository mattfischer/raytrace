use crate::geo;

use geo::Transformation;
use geo::Vec2;
use geo::Vec3;

#[derive(Copy, Clone, PartialEq)]
pub struct Bivec3 {
    pub u: Vec3,
    pub v: Vec3,
}

impl Bivec3 {
    pub const ZERO: Self = Self::new(Vec3::ZERO, Vec3::ZERO);

    pub const fn new(u: Vec3, v: Vec3) -> Self {
        Self { u, v }
    }

    pub fn transform(&self, xform: Transformation) -> Self {
        Self::new(self.u.transform(xform), self.v.transform(xform))
    }

    pub fn inverse_transform(&self, xform: Transformation) -> Self {
        Self::new(
            self.u.inverse_transform(xform),
            self.v.inverse_transform(xform),
        )
    }
}

impl std::default::Default for Bivec3 {
    fn default() -> Self {
        Self::ZERO
    }
}

impl std::ops::Add for Bivec3 {
    type Output = Self;

    fn add(self, rhs: Self) -> Self {
        Self::new(self.u + rhs.u, self.v + rhs.v)
    }
}

impl std::ops::AddAssign for Bivec3 {
    fn add_assign(&mut self, rhs: Self) {
        *self = Self::new(self.u + rhs.u, self.v + rhs.v)
    }
}

impl std::ops::Sub for Bivec3 {
    type Output = Self;

    fn sub(self, rhs: Self) -> Self {
        Self::new(self.u - rhs.u, self.v - rhs.v)
    }
}

impl std::ops::SubAssign for Bivec3 {
    fn sub_assign(&mut self, rhs: Self) {
        *self = Self::new(self.u - rhs.u, self.v - rhs.v)
    }
}

impl std::ops::Mul<f32> for Bivec3 {
    type Output = Self;

    fn mul(self, rhs: f32) -> Self {
        Self::new(self.u * rhs, self.v * rhs)
    }
}

impl std::ops::Mul<Bivec3> for f32 {
    type Output = Bivec3;

    fn mul(self, rhs: Bivec3) -> Bivec3 {
        Bivec3::new(self * rhs.u, self * rhs.v)
    }
}

impl std::ops::Mul<Vec2> for Bivec3 {
    type Output = Vec3;

    fn mul(self, rhs: Vec2) -> Vec3 {
        self.u * rhs.u + self.v * rhs.v
    }
}

impl std::ops::MulAssign<f32> for Bivec3 {
    fn mul_assign(&mut self, rhs: f32) {
        *self = Self::new(self.u * rhs, self.v * rhs)
    }
}

impl std::ops::Div<f32> for Bivec3 {
    type Output = Self;

    fn div(self, rhs: f32) -> Self {
        Self::new(self.u / rhs, self.v / rhs)
    }
}

impl std::ops::DivAssign<f32> for Bivec3 {
    fn div_assign(&mut self, rhs: f32) {
        *self = Self::new(self.u / rhs, self.v / rhs)
    }
}

impl std::ops::Neg for Bivec3 {
    type Output = Self;

    fn neg(self) -> Self {
        Self::new(-self.u, -self.v)
    }
}

#[derive(Copy, Clone, PartialEq)]
pub struct Bivec2 {
    pub u: Vec2,
    pub v: Vec2,
}

impl Bivec2 {
    pub const ZERO: Self = Self::new(Vec2::ZERO, Vec2::ZERO);

    pub const fn new(u: Vec2, v: Vec2) -> Self {
        Self { u, v }
    }
}

impl std::default::Default for Bivec2 {
    fn default() -> Self {
        Self::ZERO
    }
}

impl std::ops::Add for Bivec2 {
    type Output = Self;

    fn add(self, rhs: Self) -> Self {
        Self::new(self.u + rhs.u, self.v + rhs.v)
    }
}

impl std::ops::AddAssign for Bivec2 {
    fn add_assign(&mut self, rhs: Self) {
        *self = Self::new(self.u + rhs.u, self.v + rhs.v)
    }
}

impl std::ops::Sub for Bivec2 {
    type Output = Self;

    fn sub(self, rhs: Self) -> Self {
        Self::new(self.u - rhs.u, self.v - rhs.v)
    }
}

impl std::ops::SubAssign for Bivec2 {
    fn sub_assign(&mut self, rhs: Self) {
        *self = Self::new(self.u - rhs.u, self.v - rhs.v)
    }
}

impl std::ops::Mul<f32> for Bivec2 {
    type Output = Self;

    fn mul(self, rhs: f32) -> Self {
        Self::new(self.u * rhs, self.v * rhs)
    }
}

impl std::ops::Mul<Bivec2> for f32 {
    type Output = Bivec2;

    fn mul(self, rhs: Bivec2) -> Bivec2 {
        Bivec2::new(self * rhs.u, self * rhs.v)
    }
}

impl std::ops::Mul<Vec2> for Bivec2 {
    type Output = Vec2;

    fn mul(self, v: Vec2) -> Vec2 {
        self.u * v.u + self.v * v.v
    }
}

impl std::ops::MulAssign<f32> for Bivec2 {
    fn mul_assign(&mut self, rhs: f32) {
        *self = Self::new(self.u * rhs, self.v * rhs)
    }
}

impl std::ops::Div<f32> for Bivec2 {
    type Output = Self;

    fn div(self, rhs: f32) -> Self {
        Self::new(self.u / rhs, self.v / rhs)
    }
}

impl std::ops::DivAssign<f32> for Bivec2 {
    fn div_assign(&mut self, rhs: f32) {
        *self = Self::new(self.u / rhs, self.v / rhs)
    }
}

impl std::ops::Neg for Bivec2 {
    type Output = Self;

    fn neg(self) -> Self {
        Self::new(-self.u, -self.v)
    }
}
