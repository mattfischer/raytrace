use crate::geo;
use crate::object;

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
            for (i, part) in line.split(' ').enumerate() {
                if i < N {
                    result[i] = part.parse::<T>().unwrap_or(T::default());
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

    pub fn load(filename : String) -> Option<Box<dyn object::Shape>> {
        if filename.ends_with(".bpt") {
            return Self::load_bpt(filename);
        }

        return None;
    }
}