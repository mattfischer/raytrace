use crate::geo;

use geo::Beam;
use geo::Bivec3;
use geo::Point2;
use geo::Point3;
use geo::Ray;
use geo::Vec2;
use geo::Vec3;

use std::f32::consts::PI;

pub struct Camera {
    pub position : Point3,
    pub direction : Vec3,
    pub image_plane : Bivec3,
    pub vertical : Vec3,
    pub fov : f32,
    pub focal_length : f32,
    pub aperture_size : f32,
    pub image_size : f32
}

impl Camera {
    pub fn new(position : Point3, direction : Vec3, vertical : Vec3, fov : f32, focal_length : f32, aperture_size : f32) -> Camera {
        let image_size = (fov * PI / (2.0 * 180.0)).tan();
        let perp_vertical = (vertical - direction * (vertical * direction)).normalize();
        let horizontal = perp_vertical % direction;
        let image_plane = Bivec3::new(horizontal, perp_vertical);
        
        Camera {position, direction, image_plane, vertical, fov, focal_length, aperture_size, image_size}
    }

    fn create_ray(&self, image_point : Point2, aperture_point : Point2) -> (Ray, Bivec3) {
        let mut direction = (self.direction + self.image_plane * Vec2::from(image_point) * self.image_size);
        let length = direction.mag();
        direction /= length;

        let p = self.position + direction * self.focal_length;
        let r = aperture_point.u.sqrt();
        let phi = 2.0 * PI * aperture_point.v;
        let aperture_disc_point = Vec2::new(r * phi.cos(), r * phi.sin());
        let q = self.position + self.image_plane * aperture_disc_point * self.aperture_size;
        direction = (p - q).normalize();

        let differential = self.image_plane / length;
        let ray = Ray::new(q, direction);
        return (ray, differential);
    }
    
    pub fn create_pixel_beam(&self, image_point : Point2, width : usize, height : usize, aperture_point : Point2) -> Beam {
        let cx = (2.0 * image_point.u - width as f32) / (width as f32);
        let cy = (2.0 * image_point.v - height as f32) / (width as f32);
        let image_point_transformed = Point2::new(cx, -cy);
        let (ray, dv) = self.create_ray(image_point_transformed, aperture_point);
        
        let pixel_size = 2.0 / (width as f32);

        return Beam::new(ray, Bivec3::ZERO, dv * pixel_size);
    }
}