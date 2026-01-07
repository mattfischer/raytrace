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

pub struct Sky {
    radiance: Radiance
}

impl Sky {
    pub fn new(radiance: Radiance) -> Sky {
        return Sky { radiance };
    }
}

impl Light for Sky {
    fn sample(&self, _sampler: &mut dyn Sampler, _pnt: Point3) -> Option<LightSample>
    {
        return None;
    }

    fn pdf(&self, _isect: &Intersection) -> Pdf
    {
        return Pdf::new(0.0, false);
    }

    fn radiance_from_isect(&self, _isect: &Intersection) -> Radiance
    {
        return self.radiance;
    }

    fn radiance_from_direction(&self, _direction: Vec3) -> Radiance
    {
        return self.radiance;
    }

    fn test_visible(&self, scene: &Scene, sample: &LightSample) -> bool
    {
        let ray = Ray::new(sample.origin, sample.direction);
        let beam = Beam::new(ray, Bivec3::ZERO, Bivec3::ZERO);
        let isect = scene.intersect(beam, f32::MAX, false);

        return isect.is_none();
    }

    fn is_sky(&self) -> bool {
        return true;
    }
}