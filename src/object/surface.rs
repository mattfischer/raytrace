use crate::object;

use object::Albedo;
use object::Brdf;

#[derive(Debug)]
pub struct Surface {
    pub albedo : Box<dyn Albedo>,
    pub brdfs : Vec<Box<dyn Brdf>>
}

impl Surface {
    pub fn new(albedo : Box<dyn Albedo>, brdfs : Vec<Box<dyn Brdf>>) -> Surface {
        Surface{albedo, brdfs}
    }
}