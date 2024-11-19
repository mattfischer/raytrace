#[derive(Copy, Clone, Debug)]
pub struct Radiance {
    pub red : f32,
    pub green : f32,
    pub blue : f32
}

impl Radiance {
    pub const ZERO : Radiance = Radiance{red: 0.0, green: 0.0, blue: 0.0};
    
    pub fn new(red : f32, green : f32, blue : f32) -> Radiance {
        return Radiance {red, green, blue};
    }
}