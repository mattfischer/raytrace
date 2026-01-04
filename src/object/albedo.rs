mod solid;
pub use solid::Solid;

mod texture;
pub use texture::Texture;

use crate::geo;
use geo::Point2;
use geo::Bivec2;

use crate::object;
use object::Color;

pub trait Albedo: Send + Sync {
    fn color(&self, surface_point: Point2, surface_projection: Bivec2) -> Color;
}
