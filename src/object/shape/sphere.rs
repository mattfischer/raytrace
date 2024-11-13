use crate::geo;
use crate::object;

use geo::Normal3;
use geo::Point3;
use geo::Ray;
use geo::Transformation;

use object::BoundingVolume;
use object::Shape;
use object::ShapeIntersection;

#[derive(Debug)]
pub struct Sphere {
    position : Point3,
    radius : f32
}

impl Sphere {
    pub fn new(position : Point3, radius: f32) -> Sphere {
        Sphere{position, radius}
    }
}

impl Shape for Sphere {
    fn intersect(&self, ray : Ray, shape_isect : &mut ShapeIntersection, _closest : bool) -> bool {
        let a = ray.direction.mag2();
        let b = 2.0 * (ray.origin - self.position) * ray.direction;
        let c = (ray.origin - self.position).mag2() - self.radius * self.radius;
        let disc = b*b - 4.0*a*c;
        
        if disc >= 0.0 {
            let dist0 = (-b - f32::sqrt(disc)) / (2.0*a);
            let dist1 = (-b + f32::sqrt(disc)) / (2.0*a);
            for dist in [dist0, dist1] {
                if dist >= 0.0 && dist < shape_isect.distance {
                    shape_isect.distance = dist;
                    let point = ray.origin + ray.direction * dist;
                    shape_isect.normal = Normal3::from_vec3(point - self.position) / self.radius;
                    return true;
                }    
            }
        }

        return false;
    }

    fn bounding_volume(&self, xform : Transformation) -> BoundingVolume {
        let mut mins = [0.0; BoundingVolume::NUM_VECTORS];
        let mut maxes = [0.0; BoundingVolume::NUM_VECTORS];

        for i in 0..BoundingVolume::NUM_VECTORS {
            let vector = BoundingVolume::VECTORS[i];
            let x = self.position.to_vec3() * vector;
            let y = self.radius * vector.mag();

            mins[i] = x - y;
            maxes[i] = x + y;
        }

        return BoundingVolume::with_mins_maxes(mins, maxes);
    }
}