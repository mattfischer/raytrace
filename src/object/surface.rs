use crate::object;

#[derive(Debug)]
pub struct Surface {
    pub albedo : Box<dyn object::Albedo>
}

impl Surface {
    pub fn new(albedo : Box<dyn object::Albedo>) -> Surface {
        Surface{albedo}
    }
}