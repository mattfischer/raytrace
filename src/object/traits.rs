use crate::geo;

pub struct ShapeIntersection {
    pub distance : f32,
    pub normal : geo::Normal3
}
pub trait Shape : std::fmt::Debug {
    fn intersect(self, ray : &geo::Ray, isect : &mut ShapeIntersection, closest : bool) -> bool;
}

pub trait Albedo : std::fmt::Debug {
}