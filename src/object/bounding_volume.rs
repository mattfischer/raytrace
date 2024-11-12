use crate::geo;
use crate::object;

use geo::Point3;
use geo::Vec3;

pub const NUM_VECTORS : usize = 3;
pub const VECTORS : [Vec3; NUM_VECTORS] = [Vec3::new(1.0, 0.0, 0.0), Vec3::new(0.0, 1.0, 0.0), Vec3::new(0.0, 0.0, 1.0)];

#[derive(Debug, Copy, Clone)]
pub struct BoundingVolume {
    mins : [f32; NUM_VECTORS],
    maxes : [f32; NUM_VECTORS]
}

impl BoundingVolume {
    pub const NUM_VECTORS : usize = NUM_VECTORS;
    pub const VECTORS : [Vec3; NUM_VECTORS] = VECTORS;

    pub fn new() -> BoundingVolume {
        return BoundingVolume {mins: [f32::MAX; NUM_VECTORS], maxes: [-f32::MAX; NUM_VECTORS]};
    }

    pub fn with_mins_maxes(mins: [f32; NUM_VECTORS], maxes : [f32; NUM_VECTORS]) -> BoundingVolume {
        return BoundingVolume {mins, maxes};
    }

    pub fn include_point(&mut self, point : Point3) {
        for i in 0..NUM_VECTORS {
            let dist = point.to_vec3() * VECTORS[i];
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
}