use crate::geo;

use geo::Ray;
use geo::Bivec3;
use geo::Normal3;

pub struct Beam {
    pub ray : Ray,
    pub origin_diff : Bivec3,
    pub direction_diff : Bivec3
}

impl Beam {
    pub fn new(ray : Ray, origin_diff : Bivec3, direction_diff : Bivec3) -> Beam {
        Beam{ray, origin_diff, direction_diff}
    }

    pub fn project(&self, distance : f32, nrm : Normal3) -> Bivec3 {
        let dp = self.origin_diff;
        let dd = self.direction_diff;
        let d = self.ray.direction;
        let t = distance;

        let a = dp + dd * t;
        let dtd = Bivec3::new(d * (a.u * nrm), d * (a.v * nrm)) / (nrm * d);

        return a - dtd;
    }
}