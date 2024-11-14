#[derive(PartialEq, Clone, Copy, Debug)]
pub struct Color {
    pub red: f32,
    pub green: f32,
    pub blue: f32
}

impl Color { 
    pub const ZERO : Color = Color{red: 0.0, green: 0.0, blue: 0.0};
    pub const ONE : Color = Color{red: 1.0, green: 1.0, blue: 1.0};

    pub fn new(red: f32, green: f32, blue: f32) -> Color {
        Color {red, green, blue}
    }

    pub fn mag2(self) -> f32 {
        self.red * self.red + self.green * self.green + self.blue * self.blue
    }
}

impl std::ops::Add for Color {
    type Output = Self;

    fn add(self, other: Self) -> Self {
        Self::new(self.red + other.red, self.green + other.green, self.blue + other.blue)
    }
}

impl std::ops::AddAssign for Color {
    fn add_assign(&mut self, other: Self) {
        *self = Self::new(self.red + other.red, self.green + other.green, self.blue + other.blue);
    }
}

impl std::ops::Sub for Color {
    type Output = Self;

    fn sub(self, other: Self) -> Self {
        Self::new(self.red - other.red, self.green - other.green, self.blue - other.blue)
    }
}

impl std::ops::SubAssign for Color {
    fn sub_assign(&mut self, other: Self) {
        *self = Self::new(self.red - other.red, self.green - other.green, self.blue - other.blue);
    }
}

impl std::ops::Mul<f32> for Color {
    type Output = Self;

    fn mul(self, f: f32) -> Self {
        Self::new(self.red * f, self.green * f, self.blue * f)
    }
}

impl std::ops::MulAssign<f32> for Color {
    fn mul_assign(&mut self, f: f32) {
        *self = Self::new(self.red * f, self.green * f, self.blue * f);
    }
}

impl std::ops::Mul<Color> for f32 {
    type Output = Color;

    fn mul(self, v: Color) -> Color {
        Color::new(self * v.red, self * v.green, self * v.blue)
    }
}

impl std::ops::Mul for Color {
    type Output = Self;

    fn mul(self, other: Color) -> Self {
        Self::new(self.red * other.red, self.green * other.green, self.blue * other.blue)
    }
}

impl std::ops::Div<f32> for Color {
    type Output = Self;

    fn div(self, f: f32) -> Self {
        Self::new(self.red / f, self.green / f, self.blue / f)
    }
}

impl std::ops::DivAssign<f32> for Color {
    fn div_assign(&mut self, f: f32) {
        *self = Self::new(self.red / f, self.green / f, self.blue / f);
    }
}
