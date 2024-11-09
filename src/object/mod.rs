pub mod albedo;
pub mod brdf;
pub mod shape;

mod traits;
pub use traits::Shape;
pub use traits::ShapeIntersection;
pub use traits::Albedo;
pub use traits::Brdf;

mod camera;
pub use camera::Camera;

mod color;
pub use color::Color;

mod intersection;
pub use intersection::Intersection;

mod primitive;
pub use primitive::Primitive;

mod scene;
pub use scene::Scene;

mod surface;
pub use surface::Surface;