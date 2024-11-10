use crate::geo;
use crate::object;

use geo::Ray;

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
    fn intersect(&self, ray : &Ray, shape_isect : &mut ShapeIntersection, closest : bool) -> bool {
        let mut result = false;

        for shape in self.shapes.iter() {
            if shape.intersect(ray, shape_isect, closest) {
                result = true;
                if !closest {
                    break;
                }
            }
        }
        
        return result;
    }
}