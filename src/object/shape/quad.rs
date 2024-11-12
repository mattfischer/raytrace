use crate::geo;
use crate::object;

use geo::Bivec3;
use geo::Normal3;
use geo::Point2;
use geo::Point3;
use geo::Ray;
use geo::Vec3;

use object::Shape;
use object::ShapeIntersection;

#[derive(Debug)]
pub struct Quad {
    position : Point3,
    side1 : Vec3,
    side2 : Vec3,
    normal : Normal3
}

impl Quad {
    pub fn new(position : Point3, side1 : Vec3, side2 : Vec3) -> Quad {
        let normal = Normal3::from_vec3((side1 % side2).normalize());
        Quad{position, side1, side2, normal}
    }
}

impl Shape for Quad {
    fn intersect(&self, ray : &Ray, shape_isect : &mut ShapeIntersection, _closest : bool) -> bool {
        let distance = (ray.origin - self.position) * self.normal / (ray.direction * -self.normal);
        
        if distance >= 0.0 && distance < shape_isect.distance {
            let point = ray.origin + ray.direction * distance;
            let u = (point - self.position) * self.side1 / self.side1.mag2();
            let v = (point - self.position) * self.side2 / self.side2.mag2();
            if u >= 0.0 && u <= 1.0 && v >= 0.0 && v <= 1.0 {
                shape_isect.distance = distance;
                shape_isect.normal = self.normal;
                shape_isect.tangent = Bivec3::new(self.side1, self.side2);
                shape_isect.surface_point = Point2::new(u, v);
                return true;
            }
        }

        return false;
    }
}