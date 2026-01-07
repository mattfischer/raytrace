use crate::geo;
use geo::Beam;
use geo::Bivec3;
use geo::Point3;
use geo::Ray;
use geo::Vec3;

use crate::object;
use object::Light;
use object::Pdf;
use object::Radiance;
use object::Sampler;
use object::Scene;

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
    fn sample(&self, _sampler: &mut dyn Sampler, pnt: Point3) -> Option<(Radiance, Vec3, Pdf)>
    {
        let mut dir_in = self.position - pnt;
        let d = dir_in.mag();
        dir_in = dir_in / d;

        return Some((self.radiance, dir_in, Pdf::new(d * d, true)))
    }

    fn test_visible(&self, scene: &Scene, pnt: Point3, dir_in: Vec3) -> bool
    {
        let d = (self.position - pnt).mag();
        let ray = Ray::new(pnt, dir_in);
        let beam = Beam::new(ray, Bivec3::ZERO, Bivec3::ZERO);
        let isect = scene.intersect(beam, d, false);

        return isect.is_none();
    }
}