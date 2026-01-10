mod bezier_patch;
pub use bezier_patch::BezierPatch;

mod grid;
pub use grid::Grid;
pub use grid::GridVertex;

mod group;
pub use group::Group;

mod quad;
pub use quad::Quad;

mod sphere;
pub use sphere::Sphere;

mod transformed;
pub use transformed::Transformed;

mod triangle;
pub use triangle::Triangle;

mod triangle_mesh;
pub use triangle_mesh::TriangleMesh;
pub use triangle_mesh::TriangleMeshTriangle;
pub use triangle_mesh::TriangleMeshVertex;

use crate::geo;
use geo::Bivec3;
use geo::Normal3;
use geo::Point2;
use geo::Point3;
use geo::Ray;
use geo::Transformation;

use crate::object;
use object::BoundingVolume;
use object::Pdf;
use object::Sampler;

#[derive(Copy, Clone, Default)]
pub struct ShapeIntersection {
    pub distance: f32,
    pub normal: Normal3,
    pub tangent: Bivec3,
    pub surface_point: Point2,
}

impl ShapeIntersection {
    pub fn new(
        distance: f32,
        normal: Normal3,
        tangent: Bivec3,
        surface_point: Point2,
    ) -> ShapeIntersection {
        return ShapeIntersection {
            distance,
            normal,
            tangent,
            surface_point,
        };
    }
}

pub trait Shape: Send + Sync {
    fn intersect(&self, ray: Ray, max_distance: f32, closest: bool) -> Option<ShapeIntersection>;
    fn bounding_volume(&self, xform: &Transformation) -> BoundingVolume;

    fn sample(&self, _sampler: &mut dyn Sampler) -> Option<(Point3, Normal3, Pdf)> {
        return None;
    }

    fn pdf(&self, _pnt: Point3) -> Pdf {
        return Pdf::new(0.0, false);
    }
}
