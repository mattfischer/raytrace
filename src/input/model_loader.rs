use crate::geo;
use crate::object;

use object::shape::TriangleMesh;
use object::shape::TriangleMeshVertex;
use object::shape::TriangleMeshTriangle;

use ply_rs;
use ply_rs::ply;
use ply_rs::ply::PropertyAccess;

use geo::Normal3;
use geo::Point3;

use std::io::{BufRead, BufReader};
use std::fs::File;
use std::str::FromStr;

pub struct ModelLoader;

impl ModelLoader {
    
    fn parse_nums<T, const N : usize>(reader : &mut impl BufRead) -> [T; N]
    where T: FromStr + Copy + Default {
        let mut line = String::new();
        let mut result = [T::default(); N];
        if reader.read_line(&mut line).is_ok() {
            for (i, part) in line.trim_end().split(' ').enumerate() {
                if i < N {
                    result[i] = part.parse().unwrap_or(T::default());
                }
            }
        }
    
        return result;
    }

    fn load_bpt(filename : String) -> Option<Box<dyn object::Shape>> {
        if let Ok(file) = File::open(filename) {
            let mut reader = BufReader::new(file);
            
            let num_patches = Self::parse_nums::<i32, 1>(&mut reader)[0];

            let mut patches : Vec<Box<dyn object::Shape>> = Vec::new();
            for _ in 0..num_patches {
                let [_, _] = Self::parse_nums::<i32, 2>(&mut reader);
                let mut control_points = [Point3::ZERO; 16];
                for j in 0..16 {
                    let [x, y, z] = Self::parse_nums::<f32, 3>(&mut reader);
                    control_points[j] = Point3::new(x, y, z);
                }
                patches.push(Box::new(object::shape::BezierPatch::new(16, 16, control_points)));
            }

            return Some(Box::new(object::shape::Group::new(patches)));
        } else {
            return None;
        }
    }

    fn load_ply(filename : String) -> Option<Box<dyn object::Shape>> {
        if let Ok(mut file) = File::open(filename) {
            let parser = ply_rs::parser::Parser::<ply::DefaultElement>::new();
            let ply = parser.read_ply(&mut file);
            if let Ok(ply) = ply {
                let mut points = Vec::new();
                for elem in ply.payload["vertex"].iter() {
                    let x = elem.get_float(&String::from("x")).unwrap_or(0.0);
                    let y = elem.get_float(&String::from("y")).unwrap_or(0.0);
                    let z = elem.get_float(&String::from("z")).unwrap_or(0.0);
                    points.push(TriangleMeshVertex{point: Point3::new(x, y, z)});
                }

                let mut triangles = Vec::new();
                for elem in ply.payload["face"].iter() {
                    let ind = elem.get_list_int(&String::from("vertex_indices")).unwrap_or(&[0, 0, 0]);
                    let mut indices = [0 as usize; 3];
            
                    for i in 0..3 {
                        indices[i] = ind[i] as usize;
                    }

                    let u = points[indices[1]].point - points[indices[0]].point;
                    let v = points[indices[2]].point - points[indices[0]].point;
                    
                    let normal = Normal3::from(u % v);
                    triangles.push(TriangleMeshTriangle{vertices: indices, normal: normal});
                }

                return Some(Box::new(TriangleMesh::new(points, triangles)));
            } else {
                return None;
            }
        } else {
            return None;
        }
    }

    pub fn load(filename : String) -> Option<Box<dyn object::Shape>> {
        if filename.ends_with(".bpt") {
            return Self::load_bpt(filename);
        } else if filename.ends_with(".ply") {
            return Self::load_ply(filename);
        }

        return None;
    }
}