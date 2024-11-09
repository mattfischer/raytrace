use crate::geo;

pub struct ShapeIntersection {
    pub distance : f32,
    pub normal : geo::Normal3
}

pub trait Shape {
    fn intersect(self, ray : &geo::Ray, isect : &mut ShapeIntersection, closest : bool) -> bool;
}

pub mod shape;