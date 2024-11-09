use super::Vec3;

#[derive(PartialEq, Clone, Copy, Debug)]
pub struct Normal3 {
    pub x: f32,
    pub y: f32,
    pub z: f32
}

impl Normal3 {
    pub fn new(x: f32, y: f32, z: f32) -> Normal3 {
        Normal3 {x, y, z}
    }

    pub fn from_vec3(v : Vec3) -> Normal3 {
        Self::new(v.x, v.y, v.z)
    }
}

impl std::ops::Add for Normal3 {
    type Output = Self;

    fn add(self, other: Self) -> Self {
        Self::new(self.x + other.x, self.y + other.y, self.z + other.z)
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
        Self::new(self.x - other.x, self.y - other.y, self.z - other.z)
    }
}

impl std::ops::SubAssign for Normal3 {
    fn sub_assign(&mut self, other: Self) {
        *self = Self::new(self.x - other.x, self.y - other.y, self.z - other.z);
    }
}

impl std::ops::Mul for Normal3 {
    type Output = f32;

    fn mul(self, other: Normal3) -> f32 {
        self.x * other.x + self.y * other.y + self.z * other.z
    }
}

impl std::ops::Mul<f32> for Normal3 {
    type Output = Self;

    fn mul(self, f: f32) -> Self {
        Self::new(self.x * f, self.y * f, self.z * f)
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
        Normal3::new(self * v.x, self * v.y, self * v.z)
    }
}

impl std::ops::Div<f32> for Normal3 {
    type Output = Self;

    fn div(self, f: f32) -> Self {
        Self::new(self.x / f, self.y / f, self.z / f)
    }
}

impl std::ops::DivAssign<f32> for Normal3 {
    fn div_assign(&mut self, f: f32) {
        *self = Self::new(self.x / f, self.y / f, self.z / f);
    }
}

impl std::ops::Rem for Normal3 {
    type Output = Self;
    fn rem(self, other: Normal3) -> Self {
        Self::new(self.y * other.z - self.z * other.y,
                  self.z * other.x - self.x * other.z,
                  self.x * other.y - self.y * other.x)
    }
}

impl std::ops::Neg for Normal3 {
    type Output = Self;
    fn neg(self) -> Normal3 {
        Self::new(-self.x, -self.y, -self.z)
    }
}
