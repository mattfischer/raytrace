mod lambert;
pub use lambert::Lambert;

mod oren_nayar;
pub use oren_nayar::OrenNayar;

mod phong;
pub use phong::Phong;

mod torrance_sparrow;
pub use torrance_sparrow::TorranceSparrow;

use crate::geo;
use geo::Normal3;
use geo::Vec3;

use crate::object;
use object::Color;
use object::Sampler;

pub trait Brdf: Send + Sync {
    fn reflected(&self, dir_in: Vec3, nrm: Normal3, dir_out: Vec3, albedo: Color) -> Color;
    fn transmitted(&self, dir_in: Vec3, nrm: Normal3, albedo: Color) -> Color;
    fn lambert(&self) -> f32;

    fn sample(&self, sampler: &mut dyn Sampler, nrm: Normal3, dir_out: Vec3) -> Vec3;
    fn pdf(&self, dir_in: Vec3, nrm: Normal3, dir_out: Vec3) -> f32;

    fn opaque(&self) -> bool {
        return true;
    }
}
