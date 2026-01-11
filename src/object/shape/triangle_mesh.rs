use crate::geo;
use crate::object;

use geo::Bivec3;
use geo::Normal3;
use geo::Point2;
use geo::Point3;
use geo::Transformation;

use object::BoundingVolume;
use object::BoundingVolumeHierarchy;
use object::Shape;
use object::ShapeIntersection;

pub struct Vertex {
    pub point: Point3,
}

pub struct Triangle {
    pub vertices: [usize; 3],
    pub normal: Normal3,
}

pub struct TriangleMesh {
    vertices: Vec<Vertex>,
    triangles: Vec<Triangle>,
    bvh: BoundingVolumeHierarchy,
}

impl TriangleMesh {
    fn compute_bvh(
        vertices: &[Vertex],
        triangles: &[Triangle],
    ) -> BoundingVolumeHierarchy {
        let mut centroids = Vec::with_capacity(triangles.len());
        for triangle in triangles {
            let mut centroid = Point3::ZERO;
            for j in 0..3 {
                centroid += vertices[triangle.vertices[j]].point;
            }
            centroids.push(centroid / 3.0);
        }

        let func = |index: usize| {
            let mut volume = BoundingVolume::new();
            for i in 0..3 {
                volume.include_point(vertices[triangles[index].vertices[i]].point);
            }
            return volume;
        };

        return BoundingVolumeHierarchy::from_volumes(&centroids[..], &func);
    }

    pub fn new(
        vertices: Vec<Vertex>,
        triangles: Vec<Triangle>,
    ) -> TriangleMesh {
        let bvh = Self::compute_bvh(&vertices[..], &triangles[..]);
        return Self::new_with_bvh(vertices, triangles, bvh);
    }

    pub fn new_with_bvh(
        vertices: Vec<Vertex>,
        triangles: Vec<Triangle>,
        bvh: BoundingVolumeHierarchy,
    ) -> TriangleMesh {
        return TriangleMesh {
            vertices,
            triangles,
            bvh,
        };
    }
}

impl Shape for TriangleMesh {
    fn bounding_volume(&self, xform: &Transformation) -> BoundingVolume {
        let mut volume = BoundingVolume::new();

        for vertex in &self.vertices {
            volume.include_point(vertex.point.transform(xform));
        }

        return volume;
    }

    fn intersect(
        &self,
        ray: geo::Ray,
        max_distance: f32,
        closest: bool,
    ) -> Option<ShapeIntersection> {
        let raydata = BoundingVolume::get_raydata(ray);
        let mut shape_isect = None;

        let mut func = |index: usize, max_distance: f32| {
            let triangle = &self.triangles[index];

            let vertex0 = &self.vertices[triangle.vertices[0]];
            let vertex1 = &self.vertices[triangle.vertices[1]];
            let vertex2 = &self.vertices[triangle.vertices[2]];

            if let Some((_tu, _tv, d)) = object::shape::Triangle::intersect(
                ray,
                vertex0.point,
                vertex1.point,
                vertex2.point,
                max_distance,
            ) {
                shape_isect = Some(ShapeIntersection::new(
                    d,
                    triangle.normal,
                    Bivec3::ZERO,
                    Point2::ZERO,
                ));
                return Some(d);
            } else {
                return None;
            }
        };

        self.bvh
            .intersect(raydata, max_distance, closest, &mut func);
        return shape_isect;
    }
}
