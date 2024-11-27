use crate::geo;
use crate::object;

use geo::Bivec3;
use geo::Normal3;
use geo::Point3;
use geo::Vec3;

use object::Shape;
use object::ShapeIntersection;
use object::shape::Grid;
use object::shape::GridVertex;

pub struct BezierPatch {
    grid : Grid
}

impl BezierPatch {
    pub fn new(width : usize, height : usize, control_points : [Point3; 16]) -> BezierPatch {
        let mut vertices = Vec::new();

        for j in 0..height {
            for i in 0..width {
                let s = (i as f32) / ((width - 1) as f32);
                let t = (j as f32) / ((height - 1) as f32);
                let s1 = ((if i < width / 2 { i + 1 } else { i - 1 }) as f32) / ((width - 1) as f32);
                let t1 = ((if j < height / 2 { j + 1 } else { j - 1 }) as f32) / ((height - 1) as f32);
          
                let bs = [(1.0 - s).powi(3), 3.0 * s * (1.0 - s).powi(2), 3.0 * s.powi(2) * (1.0 - s), s.powi(3)];
                let bt = [(1.0 - t).powi(3), 3.0 * t * (1.0 - t).powi(2), 3.0 * t.powi(2) * (1.0 - t), t.powi(3)];
                let bds = [-3.0 * (1.0 - s).powi(2), 3.0 * (1.0 - s).powi(2) - 3.0 * 2.0 * s * (1.0 - s), 3.0 * 2.0 * s * (1.0 - s) - 3.0 * s.powi(2), 3.0 * s.powi(2)];
                let bdt = [-3.0 * (1.0 - t).powi(2), 3.0 * (1.0 - t).powi(2) - 3.0 * 2.0 * t * (1.0 - t), 3.0 * 2.0 * t * (1.0 - t) - 3.0 * t.powi(2), 3.0 * t.powi(2)];

                let bs1 = [(1.0 - s1).powi(3), 3.0 * s1 * (1.0 - s1).powi(2), 3.0 * s1.powi(2) * (1.0 - s1), s1.powi(3)];
                let bt1 = [(1.0 - t1).powi(3), 3.0 * t1 * (1.0 - t1).powi(2), 3.0 * t1.powi(2) * (1.0 - t1), t1.powi(3)];
                let bds1 = [-3.0 * (1.0 - s1).powi(2), 3.0 * (1.0 - s1).powi(2) - 3.0 * 2.0 * s1 * (1.0 - s1), 3.0 * 2.0 * s1 * (1.0 - s1) - 3.0 * s1.powi(2), 3.0 * s1.powi(2)];
                let bdt1 = [-3.0 * (1.0 - t1).powi(2), 3.0 * (1.0 - t1).powi(2) - 3.0 * 2.0 * t1 * (1.0 - t1), 3.0 * 2.0 * t1 * (1.0 - t1) - 3.0 * t1.powi(2), 3.0 * t1.powi(2)];

                let mut p = Vec3::ZERO;
                let mut ds = Vec3::ZERO;
                let mut dt = Vec3::ZERO;
                let mut ds1 = Vec3::ZERO;
                let mut dt1 = Vec3::ZERO;

                for k in 0..4 {
                    for l in 0..4 {
                        let c = control_points[k*4 + l].to_vec3();

                        p += c * (bs[l] * bt[k]);
                        ds += c * (bds[l] * bt[k]);
                        dt += c * (bs[l] * bdt[k]);
                        ds1 += c * bds1[l] * bt1[k];
                        dt1 += c * (bs1[l] * bdt1[k]);
                    }
                }

                if ds.mag2() < 0.0000001 {
                    ds = ds1;
                }

                if dt.mag2() < 0.0000001 {
                    dt = dt1;
                }

                let vertex = GridVertex{point: Point3::from_vec3(p), tangent: Bivec3::new(ds, dt), normal: Normal3::from_vec3((ds % dt).normalize())};
                vertices.push(vertex);
            }
        }
        
        let grid = Grid::new(width, height, vertices);
        return BezierPatch {grid};
    }
}

impl Shape for BezierPatch {
    fn bounding_volume(&self, xform : geo::Transformation) -> object::BoundingVolume {
        return self.grid.bounding_volume(xform);
    }

    fn intersect(&self, ray : geo::Ray, max_distance : f32, closest : bool) -> Option<ShapeIntersection> {
        return self.grid.intersect(ray, max_distance, closest);
    }
}