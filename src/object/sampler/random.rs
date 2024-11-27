use crate::geo;
use crate::object;

use geo::Point2;

use object::Sampler;

use rand::prelude::*;
use rand::rngs::ThreadRng;

pub struct Random {
    rng : ThreadRng
}

impl Random {
    pub fn new() -> Random {
        let rng = rand::thread_rng();
        return Random {rng};
    }
}

impl Sampler for Random {
    fn start_sample_with_index(&mut self, _index : usize) {
    }

    fn start_sample_with_xys(&mut self, _x : usize, _y : usize, _sample : usize) {
    }

    fn get_value(&mut self) -> f32 {
        return self.rng.gen();
    }
}
