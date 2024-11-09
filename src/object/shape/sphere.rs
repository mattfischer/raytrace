use crate::geo;
use crate::object;
use object::ShapeIntersection;

#[derive(Debug)]
pub struct Sphere {
    position : geo::Point3,
    radius : f32
}

impl Sphere {
    pub fn new(position : geo::Point3, radius: f32) -> Sphere {
        Sphere{position, radius}
    }
}

impl object::Shape for Sphere {
    fn intersect(self, ray : &geo::Ray, isect : &mut ShapeIntersection, _closest : bool) -> bool {
        let a = ray.direction.mag2();
        let b = 2.0 * (ray.origin - self.position) * ray.direction;
        let c = (ray.origin - self.position).mag2() - self.radius * self.radius;
        let disc = b*b - 4.0*a*c;
        
        if disc >= 0.0 {
            let dist0 = (-b - f32::sqrt(disc)) / (2.0*a);
            let dist1 = (-b + f32::sqrt(disc)) / (2.0*a);
            for dist in [dist0, dist1] {
                if dist >= 0.0 && dist < isect.distance {
                    isect.distance = dist;
                    let point = ray.origin + ray.direction * dist;
                    isect.normal = geo::Normal3::from_vec3(point - self.position) / self.radius;
                    return true;
                }    
            }
        }

        return false;

    }

}