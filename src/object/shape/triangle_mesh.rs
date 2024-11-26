use crate::geo;
use crate::object;
use crate::object::ShapeIntersection;

use geo::Bivec3;
use geo::Normal3;
use geo::Point2;
use geo::Point3;

use object::BoundingVolume;
use object::BoundingVolumeHierarchy;
use object::Shape;
use object::shape::Triangle;

#[derive(Debug)]
pub struct TriangleMeshVertex {
    pub point : Point3
}

#[derive(Debug)]
pub struct TriangleMeshTriangle {
    pub vertices : [usize; 3],
    pub normal : Normal3
}

#[derive(Debug)]
pub struct TriangleMesh {
    vertices : Vec<TriangleMeshVertex>,
    triangles : Vec<TriangleMeshTriangle>,
    bvh : BoundingVolumeHierarchy
}

impl TriangleMesh {
    fn compute_bvh(vertices : &[TriangleMeshVertex], triangles : &[TriangleMeshTriangle]) -> BoundingVolumeHierarchy {
        let mut centroids = Vec::with_capacity(triangles.len());
        for triangle in triangles {
            let mut centroid = Point3::ZERO;
            for j in 0..3 {
                centroid += vertices[triangle.vertices[j]].point;
            }
            centroids.push(centroid / 3.0);
        }
        
        let func = |index : usize| {
            let mut volume = BoundingVolume::new();
            for i in 0..3 {
                volume.include_point(vertices[triangles[index].vertices[i]].point);
            }
            return volume;
        };

        return BoundingVolumeHierarchy::with_volumes(&centroids[..], &func);
    }

    pub fn new(vertices : Vec<TriangleMeshVertex>, triangles : Vec<TriangleMeshTriangle>) -> TriangleMesh {
        let bvh = Self::compute_bvh(&vertices[..], &triangles[..]);
        return Self::new_with_bvh(vertices, triangles, bvh);
    }

    pub fn new_with_bvh(vertices : Vec<TriangleMeshVertex>, triangles : Vec<TriangleMeshTriangle>, bvh : BoundingVolumeHierarchy) -> TriangleMesh {
        return TriangleMesh {vertices, triangles, bvh};
    }
}

impl Shape for TriangleMesh {
    fn bounding_volume(&self, xform : geo::Transformation) -> BoundingVolume {
        let mut volume = BoundingVolume::new();

        for vertex in self.vertices.iter() {
            volume.include_point(vertex.point.transform(xform));
        }

        return volume;
    }

    fn intersect(&self, ray : geo::Ray, max_distance : f32, closest : bool) -> Option<ShapeIntersection> {
        let raydata = BoundingVolume::get_raydata(ray);
        let mut shape_isect = None;

        let mut func = |index : usize, max_distance : &mut f32| {
            let triangle = &self.triangles[index];

            let vertex0 = &self.vertices[triangle.vertices[0]];
            let vertex1 = &self.vertices[triangle.vertices[1]];
            let vertex2 = &self.vertices[triangle.vertices[2]];

            if let Some((_tu, _tv)) = Triangle::intersect(ray, vertex0.point, vertex1.point, vertex2.point, max_distance) {
                shape_isect = Some(ShapeIntersection::new(*max_distance, triangle.normal, Bivec3::ZERO, Point2::ZERO));
                return true;
            } else {
                return false;
            }
        };
        
        let mut distance = max_distance;
        self.bvh.intersect(raydata, &mut distance, closest, &mut func);
        return shape_isect;
    }
}