use crate::geo;
use geo::Point3;

use crate::object;
use object::Intersection;
use object::Light;
use object::Pdf;
use object::Radiance;
use object::Sampler;

pub struct Point {
    position: Point3,
    radiance: Radiance
}

impl Point {
    pub fn new(position: Point3, radiance: Radiance) -> Point {
        return Point { position, radiance };
    }
}

impl Light for Point {
    fn sample(&self, _sampler: &mut dyn Sampler, pnt: Point3) -> Option<(Radiance, Point3, Pdf)>
    {
        let dir_out = pnt - self.position;
        let d = dir_out.mag();

        return Some((self.radiance, self.position, Pdf::new(d * d, true)))
    }

    fn did_intersect(&self, _isect: &Intersection) -> bool
    {
        return false;
    }
}