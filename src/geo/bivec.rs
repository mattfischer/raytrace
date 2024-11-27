use crate::geo;

use geo::Transformation;
use geo::Vec2;
use geo::Vec3;

#[derive(Copy, Clone, PartialEq)]
pub struct Bivec3 {
    pub u : Vec3,
    pub v : Vec3
}

impl Bivec3 {
    pub const ZERO : Bivec3 = Bivec3::new(Vec3::ZERO, Vec3::ZERO);
  
    pub const fn new(u : Vec3, v : Vec3) -> Bivec3 {
        return Bivec3{u, v};
    }

    pub fn transform(&self, xform : Transformation) -> Bivec3 {
        return Self::new(self.u.transform(xform), self.v.transform(xform));
    }

    pub fn inverse_transform(&self, xform : Transformation) -> Bivec3 {
        return Self::new(self.u.inverse_transform(xform), self.v.inverse_transform(xform));
    }
}

impl std::ops::Add for Bivec3 {
    type Output = Self;

    fn add(self, other : Self) -> Self {
        return Self::new(self.u + other.u, self.v + other.v);
    }
}

impl std::ops::AddAssign for Bivec3 {
    fn add_assign(&mut self, other : Self) {
        *self = Self::new(self.u + other.u, self.v + other.v)
    }
}

impl std::ops::Sub for Bivec3 {
    type Output = Self;

    fn sub(self, other : Self) -> Self {
        return Self::new(self.u - other.u, self.v - other.v);
    }
}

impl std::ops::SubAssign for Bivec3 {
    fn sub_assign(&mut self, other : Bivec3) {
        *self = Self::new(self.u - other.u, self.v - other.v)
    }
}

impl std::ops::Mul<f32> for Bivec3 {
    type Output = Self;

    fn mul(self, f : f32) -> Self {
        return Self::new(self.u * f, self.v * f);
    }
}

impl std::ops::Mul<Bivec3> for f32 {
    type Output = Bivec3;

    fn mul(self, v : Bivec3) -> Bivec3 {
        return Bivec3::new(self * v.u, self * v.v);
    }
}

impl std::ops::Mul<Vec2> for Bivec3 {
    type Output = Vec3;

    fn mul(self, v : Vec2) -> Vec3 {
        return self.u * v.u + self.v * v.v
    }
}

impl std::ops::MulAssign<f32> for Bivec3 {
    fn mul_assign(&mut self, f: f32) {
        *self = Self::new(self.u * f, self.v * f)
    }
}

impl std::ops::Div<f32> for Bivec3 {
    type Output = Self;

    fn div(self, f : f32) -> Self {
        return Self::new(self.u / f, self.v / f);
    }
}

impl std::ops::DivAssign<f32> for Bivec3 {
    fn div_assign(&mut self, f: f32) {
        *self = Self::new(self.u / f, self.v / f)
    }
}

impl std::ops::Neg for Bivec3 {
    type Output = Self;

    fn neg(self) -> Self {
        return Self::new(-self.u, -self.v);
    }
}

#[derive(Copy, Clone, PartialEq)]
pub struct Bivec2 {
    pub u : Vec2,
    pub v : Vec2
}

impl Bivec2 {
    pub const ZERO : Bivec2 = Bivec2::new(Vec2::ZERO, Vec2::ZERO);
  
    pub const fn new(u : Vec2, v : Vec2) -> Bivec2 {
        return Bivec2{u, v};
    }
}

impl std::ops::Add for Bivec2 {
    type Output = Self;

    fn add(self, other : Self) -> Self {
        return Self::new(self.u + other.u, self.v + other.v);
    }
}

impl std::ops::AddAssign for Bivec2 {
    fn add_assign(&mut self, other : Self) {
        *self = Self::new(self.u + other.u, self.v + other.v)
    }
}

impl std::ops::Sub for Bivec2 {
    type Output = Self;

    fn sub(self, other : Self) -> Self {
        return Self::new(self.u - other.u, self.v - other.v);
    }
}

impl std::ops::SubAssign for Bivec2 {
    fn sub_assign(&mut self, other : Self) {
        *self = Self::new(self.u - other.u, self.v - other.v)
    }
}

impl std::ops::Mul<f32> for Bivec2 {
    type Output = Self;

    fn mul(self, f : f32) -> Self {
        return Self::new(self.u * f, self.v * f);
    }
}

impl std::ops::Mul<Bivec2> for f32 {
    type Output = Bivec2;

    fn mul(self, v : Bivec2) -> Bivec2 {
        return Bivec2::new(self * v.u, self * v.v);
    }
}

impl std::ops::Mul<Vec2> for Bivec2 {
    type Output = Vec2;

    fn mul(self, v : Vec2) -> Vec2 {
        return self.u * v.u + self.v * v.v
    }
}

impl std::ops::MulAssign<f32> for Bivec2 {
    fn mul_assign(&mut self, f: f32) {
        *self = Self::new(self.u * f, self.v * f)
    }
}

impl std::ops::Div<f32> for Bivec2 {
    type Output = Self;

    fn div(self, f : f32) -> Self {
        return Self::new(self.u / f, self.v / f);
    }
}

impl std::ops::DivAssign<f32> for Bivec2 {
    fn div_assign(&mut self, f: f32) {
        *self = Self::new(self.u / f, self.v / f)
    }
}

impl std::ops::Neg for Bivec2 {
    type Output = Self;

    fn neg(self) -> Self {
        return Self::new(-self.u, -self.v);
    }
}