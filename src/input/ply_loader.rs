use crate::geo;
use geo::Normal3;
use geo::Point3;

use crate::object;
use object::Shape;

use object::shape::TriangleMesh;
use object::shape::TriangleMeshVertex;
use object::shape::TriangleMeshTriangle;

use std::io::BufReader;
use std::io::BufRead;
use std::fs::File;

use std::collections::VecDeque;

pub struct PlyLoader;

struct ItemReader {
    reader: BufReader<File>,
    items: VecDeque<String>
}

impl ItemReader {
    pub fn new(reader: BufReader<File>) -> ItemReader {
        return ItemReader { reader, items: VecDeque::new() };
    }

    pub fn next(&mut self) -> String {
        if self.items.len() == 0 {
            let mut line = String::new();
            if let Ok(_size) = self.reader.read_line(&mut line) {
                for item in line.split_whitespace() {
                    self.items.push_back(item.to_string());
                }
            }
        }

        return self.items.pop_front().unwrap_or_default();
    }

    pub fn expect_item(&mut self, target: &str) -> Option<()> {
        let item = self.next();
        if item == target {
            return Some(());
        } else {
            return None;
        } 
    }

    pub fn eat_line(&mut self) {
        self.items.clear();
    }
}

struct Property {
    pub name: String,
    pub property_type: String,
    pub _list_type: String
}
struct Element {
    pub name: String,
    pub count: usize,
    pub properties: Vec<Property>
}

impl PlyLoader {
    pub fn load(filename: &str) -> Option<Box<dyn Shape>> {
        let f = File::open(filename);
        if let Err(_) = f {
            return None;
        }
        let f = f.unwrap();
        let b = BufReader::new(f);
        let mut reader = ItemReader::new(b);

        reader.expect_item("ply")?;
        reader.expect_item("format")?;
        reader.expect_item("ascii")?;
        let _ = reader.next();

        let elements = Self::parse_elements(&mut reader);
        let mut vertices = Vec::new();
        let mut triangles = Vec::new();

        for element in &elements {
            if element.name == "vertex" {
                let mut idx_x = 0;
                let mut idx_y = 0;
                let mut idx_z = 0;
                for (idx, property) in element.properties.iter().enumerate() {
                    match property.name.as_str() {
                        "x" => idx_x = idx,
                        "y" => idx_y = idx,
                        "z" => idx_z = idx,
                        _ => ()
                    }
                }

                for _ in 0..element.count {
                    let mut x = 0.0;
                    let mut y = 0.0;
                    let mut z = 0.0;
                    for j in 0..element.properties.len() {
                        if j == idx_x {
                            x = reader.next().parse::<f32>().unwrap_or_default();
                        } else if j == idx_y {
                            y = reader.next().parse::<f32>().unwrap_or_default();
                        } else if j == idx_z {
                            z = reader.next().parse::<f32>().unwrap_or_default();
                        } else {
                            Self::skip_property(&mut reader, &element.properties[j]);
                        }
                    }

                    let vertex = TriangleMeshVertex { point: Point3::new(x, y, z) };
                    vertices.push(vertex);
                }
            } else if element.name == "face" {
                let mut idx_vertices = 0;
                for (idx, property) in element.properties.iter().enumerate() {
                    if property.name == "vertex_indices" {
                        idx_vertices = idx;
                    }
                }

                for _ in 0..element.count {
                    for j in 0..element.properties.len() {
                        if j == idx_vertices {
                            let mut triangle_vertices = [0 as usize; 3];
                            let count = reader.next().parse::<usize>().unwrap_or_default();
                            for k in 0..count {
                                let index = reader.next().parse::<usize>().unwrap_or_default();
                                if k < 3 {
                                    triangle_vertices[k] = index;
                                } else {
                                    let u = vertices[triangle_vertices[1]].point - vertices[triangle_vertices[0]].point;
                                    let v = vertices[triangle_vertices[2]].point - vertices[triangle_vertices[0]].point;
                                    let normal = Normal3::from(u % v).normalize();
                                    let triangle = TriangleMeshTriangle { vertices: [triangle_vertices[0], triangle_vertices[1], triangle_vertices[2]], normal };
                                    triangles.push(triangle);
                                    triangle_vertices[1] = triangle_vertices[2];
                                    triangle_vertices[2] = index;
                                }
                            }

                            let u = vertices[triangle_vertices[1]].point - vertices[triangle_vertices[0]].point;
                            let v = vertices[triangle_vertices[2]].point - vertices[triangle_vertices[0]].point;
                            let normal = Normal3::from(u % v).normalize();
                            let triangle = TriangleMeshTriangle { vertices: [triangle_vertices[0], triangle_vertices[1], triangle_vertices[2]], normal };
                            triangles.push(triangle);
                        } else {
                            Self::skip_property(&mut reader, &element.properties[j]);
                        }
                    }
                }
            } else {
                Self::skip_element(&mut reader, &element);
            }
        }

        return Some(Box::new(TriangleMesh::new(vertices, triangles)));
    }

    fn parse_elements(reader: &mut ItemReader) -> Vec<Element> {
        let mut elements = Vec::new();
        let mut keyword = reader.next();
        while keyword != "end_header" {
            if keyword == "comment" {
                reader.eat_line();
                keyword = reader.next();
            } else if keyword == "element" {
                let name = reader.next();
                let count = reader.next().parse::<usize>().unwrap_or_default();
                let mut properties = Vec::new();
                keyword = reader.next();
                while keyword == "property" {
                    let property_type = reader.next();
                    let mut _list_type = String::new();
                    if property_type == "list" {
                        let _index_type = reader.next();
                        _list_type = reader.next();
                    }
                    let name = reader.next();
                    let property = Property { name, property_type, _list_type };
                    properties.push(property);
                    keyword = reader.next();
                }
                let element = Element { name, count, properties };
                elements.push(element);
            }
        }

        return elements;
    }

    fn skip_property(reader: &mut ItemReader, property: &Property) {
        if property.property_type == "list" {
            let count = reader.next().parse::<usize>().unwrap_or_default();
            for _ in 0..count {
                let _item = reader.next();
            }
        } else {
            let _item = reader.next();
        }
    }
    
    fn skip_element(reader: &mut ItemReader, element: &Element) {
        for _ in 0..element.count {
            for property in &element.properties {
                Self::skip_property(reader, property);
            }
        }
    }
}