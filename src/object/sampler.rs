use crate::geo;

use geo::Point2;

mod halton;
pub use halton::Halton;

mod random;
pub use random::Random;

pub trait Sampler {
    fn start_sample_with_index(&mut self, index: usize);
    fn start_sample_with_xys(&mut self, x: usize, y: usize, sample: usize);

    fn get_value(&mut self) -> f32;

    fn get_value2(&mut self) -> geo::Point2 {
        return Point2::new(self.get_value(), self.get_value());
    }
}
