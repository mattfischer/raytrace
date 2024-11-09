#[derive(PartialEq, Clone, Copy, Debug)]
pub struct Point3 {
    pub x: f32,
    pub y: f32,
    pub z: f32
}

impl Point3 {
    pub fn new(x: f32, y: f32, z: f32) -> Point3 {
        Point3 {x, y, z}
    }
}

use super::Vec3;

impl std::ops::Add<Vec3> for Point3 {
    type Output = Self;

    fn add(self, other: Vec3) -> Self {
        Self::new(self.x + other.x, self.y + other.y, self.z + other.z)
    }
}

impl std::ops::AddAssign<Vec3> for Point3 {
    fn add_assign(&mut self, other: Vec3) {
        *self = Self::new(self.x + other.x, self.y + other.y, self.z + other.z);
    }
}

impl std::ops::Sub<Vec3> for Point3 {
    type Output = Self;

    fn sub(self, other: Vec3) -> Self {
        Self::new(self.x - other.x, self.y - other.y, self.z - other.z)
    }
}

impl std::ops::Sub for Point3 {
    type Output = Vec3;

    fn sub(self, other: Self) -> Vec3 {
        Vec3::new(self.x - other.x, self.y - other.y, self.z - other.z)
    }
}

impl std::ops::SubAssign<Vec3> for Point3 {
    fn sub_assign(&mut self, other: Vec3) {
        *self = Self::new(self.x - other.x, self.y - other.y, self.z - other.z);
    }
}
