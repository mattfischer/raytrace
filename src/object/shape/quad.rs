use crate::geo;
use crate::object;

use geo::Bivec3;
use geo::Normal3;
use geo::Point2;
use geo::Point3;
use geo::Ray;
use geo::Transformation;
use geo::Vec3;

use object::BoundingVolume;
use object::Sampler;
use object::Shape;
use object::ShapeIntersection;

pub struct Quad {
    position : Point3,
    side1 : Vec3,
    side2 : Vec3,
    normal : Normal3
}

impl Quad {
    pub fn new(position : Point3, side1 : Vec3, side2 : Vec3) -> Quad {
        let normal = Normal3::from((side1 % side2).normalize());
        Quad{position, side1, side2, normal}
    }
}

impl Shape for Quad {
    fn intersect(&self, ray : Ray, max_distance : f32, _closest : bool) -> Option<ShapeIntersection> {
        let distance = (ray.origin - self.position) * self.normal / (ray.direction * -self.normal);
        
        if distance >= 0.0 && distance < max_distance {
            let point = ray.origin + ray.direction * distance;
            let u = (point - self.position) * self.side1 / self.side1.mag2();
            let v = (point - self.position) * self.side2 / self.side2.mag2();
            if u >= 0.0 && u <= 1.0 && v >= 0.0 && v <= 1.0 {
                return Some(ShapeIntersection::new(distance, self.normal, Bivec3::new(self.side1, self.side2), Point2::new(u, v)));
            }
        }

        return None;
    }

    fn bounding_volume(&self, xform : Transformation) -> BoundingVolume {
        let mut volume = BoundingVolume::new();
        for point in [self.position, self.position + self.side1, self.position + self.side2, self.position + self.side1 + self.side2] {
            volume.include_point(point.transform(xform));
        }

        return volume;
    }

    fn sample(&self, sampler : &mut dyn Sampler) -> Option<(Point3, Normal3, f32)> {
        let pnt_surface = sampler.get_value2();
        let pnt = self.position + self.side1 * pnt_surface.u + self.side2 * pnt_surface.v;
        let nrm = self.normal;

        let surface_area = (self.side1 % self.side2).mag();
        let pdf = 1.0 / surface_area;

        return Some((pnt, nrm, pdf));        
    }

    fn sample_pdf(&self, _pnt : Point3) -> f32 {
        let surface_area = (self.side1 % self.side2).mag();
        return 1.0 / surface_area;
    }
}