use crate::geo;

use geo::Point3;
use geo::Ray;
use geo::Vec3;

pub const NUM_VECTORS : usize = 3;

#[derive(Debug, Copy, Clone)]
pub struct BoundingVolume {
    mins : [f32; NUM_VECTORS],
    maxes : [f32; NUM_VECTORS]
}

#[derive(Copy, Clone)]
pub struct RayData {
    offsets : [f32; NUM_VECTORS],
    dots : [f32; NUM_VECTORS]
}

impl BoundingVolume {
    pub const NUM_VECTORS : usize = NUM_VECTORS;
    pub const VECTORS : [Vec3; NUM_VECTORS] = [Vec3::new(1.0, 0.0, 0.0), Vec3::new(0.0, 1.0, 0.0), Vec3::new(0.0, 0.0, 1.0)];

    pub fn new() -> BoundingVolume {
        return BoundingVolume {mins: [f32::MAX; NUM_VECTORS], maxes: [-f32::MAX; NUM_VECTORS]};
    }

    pub fn with_mins_maxes(mins: [f32; NUM_VECTORS], maxes : [f32; NUM_VECTORS]) -> BoundingVolume {
        return BoundingVolume {mins, maxes};
    }

    pub fn include_point(&mut self, point : Point3) {
        for i in 0..NUM_VECTORS {
            let dist = point.to_vec3() * Self::VECTORS[i];
            self.mins[i] = self.mins[i].min(dist);
            self.maxes[i] = self.maxes[i].max(dist);
        }
    }

    pub fn include_volume(&mut self, volume : BoundingVolume) {
        for i in 0..NUM_VECTORS {
            self.mins[i] = self.mins[i].min(volume.mins[i]);
            self.maxes[i] = self.maxes[i].max(volume.maxes[i]);
        }
    }

    pub fn centroid(&self) -> Point3 {
        let d0 = (self.mins[0] + self.maxes[0]) / 2.0;
        let d1 = (self.mins[1] + self.maxes[1]) / 2.0;
        let d2 = (self.mins[2] + self.maxes[2]) / 2.0;
        
        let v0 = Self::VECTORS[0];
        let v1 = Self::VECTORS[1];
        let v2 = Self::VECTORS[2];

        let d = v0 * (v1 % v2);
        
        return Point3::from_vec3((v1 % v2) * (d0 / d) + (v2 % v0) * (d1 / d) + (v0 % v1) * (d2 / d));    
    }

    pub fn get_raydata(ray : Ray) -> RayData {
        let mut offsets = [0.0; NUM_VECTORS];
        let mut dots = [0.0; NUM_VECTORS];
        
        for i in 0..NUM_VECTORS {
            offsets[i] = ray.origin.to_vec3() * Self::VECTORS[i];
            dots[i] = ray.direction * Self::VECTORS[i];
        }

        return RayData {offsets, dots};
    }

    pub fn intersect_ray(&self, raydata : RayData) -> Option<(f32, f32)> {
        let mut current_min = -f32::MAX;
        let mut current_max = f32::MAX;

        for i in 0..NUM_VECTORS {
            let offset = raydata.offsets[i];
            let dot = raydata.dots[i];

            let min : f32;
            let max : f32;

            if dot < 0.0 {
                max = (self.mins[i] - offset) / dot;
                min = (self.maxes[i] - offset) / dot;
            } else if dot == 0.0 {
                if offset > self.maxes[i] || offset < self.mins[i] {
                    return None;
                } else {
                    continue;
                }
            } else {
                min = (self.mins[i] - offset) / dot;
                max = (self.maxes[i] - offset) / dot; 
            }

            current_min = current_min.max(min);
            current_max = current_max.min(max);

            if current_min > current_max || current_max < 0.0 {
                return None;
            }
        }

        if current_min > current_max || current_max < 0.0 {
            return None;
        }

        return Some((current_min, current_max));
    }
}