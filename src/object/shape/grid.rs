use crate::geo;
use crate::geo::Transformation;
use crate::object;

use geo::Bivec3;
use geo::Normal3;
use geo::Point2;
use geo::Point3;
use geo::Ray;

use object::BoundingVolume;
use object::BoundingVolumeHierarchy;
use object::BvhNode;
use object::Shape;
use object::ShapeIntersection;
use object::shape::Triangle;

#[derive(Copy, Clone)]
pub struct GridVertex {
    pub point : Point3,
    pub normal : Normal3,
    pub tangent : Bivec3
}

pub struct Grid {
    width : usize,
    height : usize,
    vertices : Vec<GridVertex>,
    bvh : BoundingVolumeHierarchy
}

impl Grid {
    fn compute_bounds(vertices : &[GridVertex], width : usize, height : usize, nodes : &mut Vec<BvhNode>, u_min : usize, v_min : usize, u_max : usize, v_max : usize) -> usize {
        nodes.push(BvhNode{volume: BoundingVolume::new(), index: 0});
        let node_index = nodes.len() - 1;
        if u_max - u_min == 1 && v_max - v_min == 1 {
            let node = &mut nodes[node_index];
        
            node.index = -((v_min * width + u_min) as i32);
            for i in u_min..=u_max {
                for j in v_min..=v_max {
                    node.volume.include_point(vertices[j*width + i].point);
                }
            }
        } else {
            let new_index : usize;
            if u_max - u_min >= v_max - v_min {
                let u_split = (u_min + u_max) / 2;
                Self::compute_bounds(vertices, width, height, nodes, u_min, v_min, u_split, v_max);
                new_index = Self::compute_bounds(vertices, width, height, nodes, u_split, v_min, u_max, v_max);
            } else {
                let v_split = (v_min + v_max) / 2;
                Self::compute_bounds(vertices, width, height, nodes, u_min, v_min, u_max, v_split);
                new_index = Self::compute_bounds(vertices, width, height, nodes, u_min, v_split, u_max, v_max);
            }

            nodes[node_index].index = new_index as i32;
            
            let volume0 = nodes[node_index + 1].volume;
            nodes[node_index].volume.include_volume(volume0);

            let volume1 = nodes[new_index].volume;
            nodes[node_index].volume.include_volume(volume1);
        }

        return node_index;
    }

    pub fn new(width : usize, height : usize, vertices : Vec<GridVertex>) -> Grid {
        let mut nodes = Vec::new();
        nodes.reserve(width * height * 2);
        Self::compute_bounds(&vertices[..], width, height, &mut nodes, 0, 0, width - 1, height - 1);

        let bvh = BoundingVolumeHierarchy::with_nodes(nodes);
        return Grid{width, height, vertices, bvh};
    }

    pub fn vertex(&self, u : usize, v : usize) -> GridVertex {
        return self.vertices[v * self.width + u];
    }
}

impl Shape for Grid {
    fn bounding_volume(&self, xform : Transformation) -> BoundingVolume {
        let mut volume = BoundingVolume::new();

        for vertex in self.vertices.iter() {
            volume.include_point(vertex.point.transform(xform));
        }

        return volume;
    }

    fn intersect(&self, ray : Ray, max_distance : f32, closest : bool) -> Option<ShapeIntersection> {
        let raydata = BoundingVolume::get_raydata(ray);
        let mut shape_isect = None;

        let mut func = |index : usize, max_distance : f32| {
            let mut result = None;
            let mut distance = max_distance;

            let u = index % self.width;
            let v = index / self.width;
            let vertex0 = self.vertex(u, v);
            let surface_point0 = Point2::new((u as f32) / (self.width as f32), (v as f32) / (self.height as f32));
            let vertex1 = self.vertex(u + 1, v);
            let surface_point1 = Point2::new(((u + 1) as f32) / (self.width as f32), (v as f32) / (self.height as f32));
            let vertex2 = self.vertex(u, v + 1);
            let surface_point2 = Point2::new((u as f32) / (self.width as f32), ((v + 1) as f32) / (self.height as f32));
            let vertex3 = self.vertex(u + 1, v + 1);
            let surface_point3 = Point2::new(((u + 1) as f32) / (self.width as f32), ((v + 1) as f32) / (self.height as f32));
            
            if let Some((tu, tv, d)) = Triangle::intersect(ray, vertex0.point, vertex1.point, vertex2.point, distance) {
                let normal = vertex0.normal * (1.0 - tu - tv) + vertex1.normal * tu + vertex2.normal * tv;
                let tangent = vertex0.tangent * (1.0 - tu - tv) + vertex1.tangent * tu + vertex2.tangent * tv;
                let surface_point = surface_point0 * (1.0 - tu - tv) + surface_point1 * tu + surface_point2 * tv;

                distance = d;
                shape_isect = Some(ShapeIntersection::new(distance, normal, tangent, surface_point));
                result = Some(distance);
            }

            if let Some((tu, tv, d)) = Triangle::intersect(ray, vertex3.point, vertex2.point, vertex1.point, distance) {
                let normal = vertex3.normal * (1.0 - tu - tv) + vertex2.normal * tu + vertex1.normal * tv;
                let tangent = vertex3.tangent * (1.0 - tu - tv) + vertex2.tangent * tu + vertex1.tangent * tv;
                let surface_point = surface_point3 * (1.0 - tu - tv) + surface_point2 * tu + surface_point1 * tv;

                distance = d;
                shape_isect = Some(ShapeIntersection::new(distance, normal, tangent, surface_point));
                result = Some(distance);
            }

            return result;
        };

        self.bvh.intersect(raydata, max_distance, closest, &mut func);

        return shape_isect;
    }
}