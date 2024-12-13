use crate::geo;
use crate::object;

use geo::Point3;

use object::Radiance;

pub struct PointLight {
    pub position: Point3,
    pub radiance: Radiance,
}

impl PointLight {
    pub fn new(position: Point3, radiance: Radiance) -> PointLight {
        return PointLight { position, radiance };
    }
}
