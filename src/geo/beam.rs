use crate::geo;

use geo::Ray;
use geo::Bivec3;

pub struct Beam {
    ray : Ray,
    origin_diff : Bivec3,
    direction_diff : Bivec3
}

impl Beam {
    pub fn new(ray : Ray, origin_diff : Bivec3, direction_diff : Bivec3) -> Beam {
        Beam{ray, origin_diff, direction_diff}
    }
}