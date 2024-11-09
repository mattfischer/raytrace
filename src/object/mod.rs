pub mod albedo;
pub mod shape;

mod traits;
pub use traits::Shape;
pub use traits::ShapeIntersection;
pub use traits::Albedo;

mod color;
pub use color::Color;

mod primitive;
pub use primitive::Primitive;

mod surface;
pub use surface::Surface;