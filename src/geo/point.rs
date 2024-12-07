use crate::geo;

use geo::Transformation;
use geo::Vec2;
use geo::Vec3;

#[derive(PartialEq, Clone, Copy)]
pub struct Point3 {
    pub x: f32,
    pub y: f32,
    pub z: f32
}

impl Point3 {
    pub const ZERO : Self = Self::new(0.0, 0.0, 0.0);
  
    pub const fn new(x: f32, y: f32, z: f32) -> Self {
        Self {x, y, z}
    }

    pub fn transform(self, xform : Transformation) -> Self {
        xform.matrix * self
    }

    pub fn inverse_transform(self, xform : Transformation) -> Self {
        xform.inverse_matrix * self
    }
}

impl std::default::Default for Point3 {
    fn default() -> Self {
        Self::ZERO
    }
}

impl std::convert::From<Vec3> for Point3 {
    fn from(value: Vec3) -> Self {
        Self::new(value.x, value.y, value.z)
    }
}

impl std::convert::From<Point3> for Vec3 {
    fn from(value: Point3) -> Self {
        Self::new(value.x, value.y, value.z)
    }
}

impl std::ops::Add<Vec3> for Point3 {
    type Output = Self;

    fn add(self, rhs: Vec3) -> Self {
        Self::new(self.x + rhs.x, self.y + rhs.y, self.z + rhs.z)
    }
}

impl std::ops::AddAssign<Vec3> for Point3 {
    fn add_assign(&mut self, rhs: Vec3) {
        *self = Self::new(self.x + rhs.x, self.y + rhs.y, self.z + rhs.z);
    }
}

impl std::ops::AddAssign for Point3 {
    fn add_assign(&mut self, rhs: Point3) {
        *self = Self::new(self.x + rhs.x, self.y + rhs.y, self.z + rhs.z);
    }
}

impl std::ops::Add for Point3 {
    type Output = Self;

    fn add(self, rhs: Point3) -> Self {
        Self::new(self.x + rhs.x, self.y + rhs.y, self.z + rhs.z)
    }
}

impl std::ops::Sub<Vec3> for Point3 {
    type Output = Self;

    fn sub(self, rhs: Vec3) -> Self {
        Self::new(self.x - rhs.x, self.y - rhs.y, self.z - rhs.z)
    }
}

impl std::ops::Sub for Point3 {
    type Output = Vec3;

    fn sub(self, rhs: Self) -> Vec3 {
        Vec3::new(self.x - rhs.x, self.y - rhs.y, self.z - rhs.z)
    }
}

impl std::ops::SubAssign<Vec3> for Point3 {
    fn sub_assign(&mut self, rhs: Vec3) {
        *self = Self::new(self.x - rhs.x, self.y - rhs.y, self.z - rhs.z);
    }
}


impl std::ops::Div<f32> for Point3 {
    type Output = Self;

    fn div(self, rhs: f32) -> Self {
        Self::new(self.x / rhs, self.y / rhs, self.z / rhs)
    }
}

#[derive(PartialEq, Clone, Copy)]
pub struct Point2 {
    pub u: f32,
    pub v: f32,
}

impl Point2 {
    pub const ZERO : Point2 = Point2::new(0.0, 0.0);
  
    pub const fn new(u: f32, v: f32) -> Point2 {
        return Point2 {u, v};
    }
}

impl std::default::Default for Point2 {
    fn default() -> Self {
        Self::ZERO
    }
}

impl std::convert::From<Point2> for Vec2 {
    fn from(value: Point2) -> Self {
        Self::new(value.u, value.v)
    }
}

impl std::ops::Add<Point2> for Point2 {
    type Output = Self;

    fn add(self, rhs: Point2) -> Self {
        Self::new(self.u + rhs.u, self.v + rhs.v)
    }
}

impl std::ops::Add<Vec2> for Point2 {
    type Output = Self;

    fn add(self, rhs: Vec2) -> Self {
        Self::new(self.u + rhs.u, self.v + rhs.v)
    }
}

impl std::ops::AddAssign<Vec2> for Point2 {
    fn add_assign(&mut self, rhs: Vec2) {
        *self = Self::new(self.u + rhs.u, self.v + rhs.v);
    }
}

impl std::ops::Sub<Vec2> for Point2 {
    type Output = Self;

    fn sub(self, rhs: Vec2) -> Self {
        Self::new(self.u - rhs.u, self.v - rhs.v)
    }
}

impl std::ops::Mul<f32> for Point2 {
    type Output = Self;

    fn mul(self, f: f32) -> Self {
        Self::new(self.u * f, self.v * f)
    }
}
