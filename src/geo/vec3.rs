#[derive(PartialEq, Clone, Copy, Debug)]
pub struct Vec3 {
    pub x: f32,
    pub y: f32,
    pub z: f32
}

impl Vec3 {
    pub fn new(x: f32, y: f32, z: f32) -> Vec3 {
        Vec3 {x, y, z}
    }

    pub fn mag2(self) -> f32 {
        self * self
    }
}

impl std::ops::Add for Vec3 {
    type Output = Self;

    fn add(self, other: Self) -> Self {
        Self::new(self.x + other.x, self.y + other.y, self.z + other.z)
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
        Self::new(self.x - other.x, self.y - other.y, self.z - other.z)
    }
}

impl std::ops::SubAssign for Vec3 {
    fn sub_assign(&mut self, other: Self) {
        *self = Self::new(self.x - other.x, self.y - other.y, self.z - other.z);
    }
}

impl std::ops::Mul for Vec3 {
    type Output = f32;

    fn mul(self, other: Vec3) -> f32 {
        self.x * other.x + self.y * other.y + self.z * other.z
    }
}

impl std::ops::Mul<f32> for Vec3 {
    type Output = Self;

    fn mul(self, f: f32) -> Self {
        Self::new(self.x * f, self.y * f, self.z * f)
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
        Vec3::new(self * v.x, self * v.y, self * v.z)
    }
}

impl std::ops::Div<f32> for Vec3 {
    type Output = Self;

    fn div(self, f: f32) -> Self {
        Self::new(self.x / f, self.y / f, self.z / f)
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
        Self::new(self.y * other.z - self.z * other.y,
                  self.z * other.x - self.x * other.z,
                  self.x * other.y - self.y * other.x)
    }
}

impl std::ops::Neg for Vec3 {
    type Output = Self;
    fn neg(self) -> Vec3 {
        Self::new(-self.x, -self.y, -self.z)
    }
}
