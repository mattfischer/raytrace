use crate::geo;
use crate::object;

use geo::Ray;
use geo::Transformation;

use object::BoundingVolume;
use object::Shape;
use object::ShapeIntersection;

#[derive(Debug)]
pub struct Group {
    shapes : Vec<Box<dyn Shape>>
}

impl Group {
    pub fn new(shapes : Vec<Box<dyn Shape>>) -> Group {
        Group{shapes}
    }
}

impl Shape for Group {
    fn intersect(&self, ray : Ray, max_distance : f32, closest : bool) -> Option<ShapeIntersection> {
        let mut result = None;
        let mut distance = max_distance;
        for shape in self.shapes.iter() {
            if let Some(shape_isect) = shape.intersect(ray, distance, closest) {
                result = Some(shape_isect);
                distance = shape_isect.distance;
                if !closest {
                    break;
                }
            }
        }
        
        return result;
    }

    fn bounding_volume(&self, xform : Transformation) -> BoundingVolume {
        let mut volume = BoundingVolume::new();

        for shape in self.shapes.iter() {
            volume.include_volume(shape.bounding_volume(xform));
        }
        
        return volume;
    }
}