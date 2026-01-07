use crate::geo;
use geo::Beam;
use geo::Bivec3;
use geo::Point3;
use geo::Ray;
use geo::Vec3;

use crate::object;
use object::Intersection;
use object::Light;
use object::LightSample;
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
    fn sample(&self, _sampler: &mut dyn Sampler, pnt: Point3) -> Option<LightSample>
    {
        let mut dir_in = self.position - pnt;
        let d = dir_in.mag();
        dir_in = dir_in / d;

        return Some(LightSample { radiance: self.radiance, origin: pnt, direction: dir_in, pdf: Pdf::new(d * d, true), distance: d } );
    }

    fn pdf(&self, _isect: &Intersection) -> Pdf
    {
        return Pdf::new(0.0, false);
    }

    fn radiance(&self, _isect: &Intersection) -> Radiance
    {
        return Radiance::ZERO;
    }

    fn test_visible(&self, scene: &Scene, sample: &LightSample) -> bool
    {
        let ray = Ray::new(sample.origin, sample.direction);
        let beam = Beam::new(ray, Bivec3::ZERO, Bivec3::ZERO);
        let isect = scene.intersect(beam, sample.distance, false);

        return isect.is_none();
    }
}