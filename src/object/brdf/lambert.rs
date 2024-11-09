use crate::object;

#[derive(Debug)]
pub struct Lambert {
    pub strength : f32
}

impl Lambert {
    pub fn new(strength : f32) -> Lambert {
        Lambert {strength}
    }
}

impl object::Brdf for Lambert {
}