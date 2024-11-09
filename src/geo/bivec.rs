use crate::geo;

use geo::Vec3;

#[derive(Debug, Copy, Clone, PartialEq)]
pub struct Bivec3 {
    pub u : Vec3,
    pub v : Vec3
}

impl Bivec3 {
    pub fn new(u : Vec3, v : Vec3) -> Bivec3 {
        Bivec3{u, v}
    }
}

impl std::ops::Add for Bivec3 {
    type Output = Self;

    fn add(self, other : Bivec3) -> Bivec3 {
        Self::new(self.u + other.u, self.v + other.v)
    }
}

impl std::ops::AddAssign for Bivec3 {
    fn add_assign(&mut self, other : Bivec3) {
        *self = Self::new(self.u + other.u, self.v + other.v)
    }
}

impl std::ops::Sub for Bivec3 {
    type Output = Self;

    fn sub(self, other : Bivec3) -> Bivec3 {
        Self::new(self.u - other.u, self.v - other.v)
    }
}

impl std::ops::SubAssign for Bivec3 {
    fn sub_assign(&mut self, other : Bivec3) {
        *self = Self::new(self.u - other.u, self.v - other.v)
    }
}

impl std::ops::Mul<f32> for Bivec3 {
    type Output = Self;

    fn mul(self, f : f32) -> Bivec3 {
        Self::new(self.u * f, self.v * f)
    }
}

impl std::ops::Mul<Bivec3> for f32 {
    type Output = Bivec3;

    fn mul(self, v : Bivec3) -> Bivec3 {
        Bivec3::new(self * v.u, self * v.v)
    }
}

impl std::ops::MulAssign<f32> for Bivec3 {
    fn mul_assign(&mut self, f: f32) {
        *self = Self::new(self.u * f, self.v * f)
    }
}

impl std::ops::Div<f32> for Bivec3 {
    type Output = Self;

    fn div(self, f : f32) -> Bivec3 {
        Self::new(self.u / f, self.v / f)
    }
}

impl std::ops::DivAssign<f32> for Bivec3 {
    fn div_assign(&mut self, f: f32) {
        *self = Self::new(self.u / f, self.v / f)
    }
}

impl std::ops::Neg for Bivec3 {
    type Output = Bivec3;

    fn neg(self) -> Bivec3 {
        Self::new(-self.u, -self.v)
    }
}