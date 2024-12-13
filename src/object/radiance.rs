use crate::object;

use object::Color;

#[derive(Copy, Clone)]
pub struct Radiance {
    pub red: f32,
    pub green: f32,
    pub blue: f32,
}

impl Radiance {
    pub const ZERO: Radiance = Radiance {
        red: 0.0,
        green: 0.0,
        blue: 0.0,
    };

    pub fn new(red: f32, green: f32, blue: f32) -> Radiance {
        return Radiance { red, green, blue };
    }

    pub fn mag2(self) -> f32 {
        return self * self;
    }

    pub fn mag(self) -> f32 {
        return self.mag2().sqrt();
    }
}

impl Default for Radiance {
    fn default() -> Self {
        return Self::ZERO;
    }
}

impl std::ops::Add for Radiance {
    type Output = Self;

    fn add(self, other: Self) -> Self::Output {
        return Self::new(
            self.red + other.red,
            self.green + other.green,
            self.blue + other.blue,
        );
    }
}

impl std::ops::AddAssign for Radiance {
    fn add_assign(&mut self, other: Self) {
        *self = Self::new(
            self.red + other.red,
            self.green + other.green,
            self.blue + other.blue,
        );
    }
}

impl std::ops::Mul<f32> for Radiance {
    type Output = Self;

    fn mul(self, f: f32) -> Self::Output {
        return Self::new(self.red * f, self.green * f, self.blue * f);
    }
}

impl std::ops::Mul<Color> for Radiance {
    type Output = Self;

    fn mul(self, c: Color) -> Self {
        return Self::new(self.red * c.red, self.green * c.green, self.blue * c.blue);
    }
}

impl std::ops::Mul for Radiance {
    type Output = f32;

    fn mul(self, other: Radiance) -> f32 {
        return self.red * other.red + self.green * other.green + self.blue * other.blue;
    }
}

impl std::ops::Div<f32> for Radiance {
    type Output = Self;

    fn div(self, f: f32) -> Self::Output {
        return Self::new(self.red / f, self.green / f, self.blue / f);
    }
}
