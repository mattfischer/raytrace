use crate::geo;

use geo::Point3;
use geo::Vec3;

#[derive(Debug)]
pub struct Camera {
    pub position : Point3,
    pub direction : Vec3,
    pub vertical : Vec3,
    pub fov : f32,
    pub focal_length : f32,
    pub aperture_size : f32
}

impl Camera {
    pub fn new(position : Point3, direction : Vec3, vertical : Vec3, fov : f32, focal_length : f32, aperture_size : f32) -> Camera {
        Camera {position, direction, vertical, fov, focal_length, aperture_size}
    }
}