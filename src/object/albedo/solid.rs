use crate::object;

#[derive(Debug)]
pub struct Solid {
    color : object::Color
}

impl Solid {
    pub fn new(color: object::Color) -> Solid {
        Solid{color}
    }
}

impl object::Albedo for Solid {

}