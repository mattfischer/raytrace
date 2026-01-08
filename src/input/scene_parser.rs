use crate::geo;
use geo::Point3;
use geo::Transformation;
use geo::Vec3;

use crate::object;
use object::Albedo;
use object::Brdf;
use object::Scene;
use object::Camera;
use object::Color;
use object::Light;
use object::NormalMap;
use object::Primitive;
use object::Radiance;
use object::Shape;
use object::Surface;

use object::albedo::Solid;
use object::albedo::Texture;

use object::brdf::Lambert;
use object::brdf::OrenNayar;
use object::brdf::Phong;
use object::brdf::TorranceSparrow;

use object::shape::Sphere;
use object::shape::Quad;
use object::shape::Transformed;

use crate::input;
use input::BmpLoader;
use input::BptLoader;
use input::PlyLoader;

use std::fs;
use std::cmp::min;

pub struct SceneParser {
    data: String,
    pos: usize,
    line: usize
}

struct ParseError {
    message: String
}

impl ParseError {
    pub fn new(message: &str) -> ParseError {
        return ParseError { message: message.to_string() };
    }
}

impl SceneParser {
    pub fn new(filename: String) -> SceneParser
    {
        let data = fs::read_to_string(filename).unwrap_or_default();
        return SceneParser { data, pos: 0, line: 1 };
    }

    pub fn parse(&mut self) -> Option<Scene> {
        if self.data.is_empty() {
            return None;
        }

        self.skip_whitespace();
        match self.parse_scene() {
            Ok(scene) => return Some(scene),
            Err(err) => {
                println!("Error, line {} : {}", self.line, err.message);
                return None;
            }
        }
    }

    fn skip_whitespace(&mut self) {
        while self.pos < self.data.len() {
            let c = self.data.chars().nth(self.pos).unwrap_or_default();
            if char::is_whitespace(c) || c == '\r' {
                self.pos += 1;
                continue;
            } else if c == '\n' {
                self.line += 1;
                self.pos += 1;
            } else {
                break;
            }
        }
    }

    fn match_end(&mut self) -> bool {
        return self.pos == self.data.len();
    }

    fn match_literal(&mut self, literal: &str) -> bool {
        if self.match_end() {
            return false;
        }

        let end = min(self.data.len(), self.pos + literal.len());
        if &self.data[self.pos..end] == literal {
            self.pos += literal.len();
            self.skip_whitespace();

            return true;
        }

        return false;
    }

    fn expect_literal(&mut self, literal: &str) -> Result<(), ParseError> {
        if !self.match_literal(literal) {
            return Err(ParseError::new(&format!("Expected: {}", literal)));
        } else {
            return Ok(());
        }
    }

    fn expect_left_brace(&mut self) -> Result<(), ParseError> {
        return self.expect_literal("{");
    }

    fn expect_right_brace(&mut self) -> Result<(), ParseError> {
        return self.expect_literal("}");
    }

    fn match_right_brace(&mut self) -> bool {
        return self.match_literal("}");
    }

    fn error_unexpected(&mut self) -> Result<(), ParseError> {
        return Err(ParseError::new("Unexpected token"));
    }

    fn parse_float(&mut self) -> Result<f32, ParseError> {
        let c = self.data.chars().nth(self.pos).unwrap_or_default();
        if self.match_end() || !(char::is_digit(c, 10) || c == '-' || c == '.') {
            return Err(ParseError::new("Expected <float>"));
        }

        let mut pos = self.pos + 1;
        while pos < self.data.len() {
            let c = self.data.chars().nth(pos).unwrap_or_default();
            if char::is_digit(c, 10) || c == '.' || c == '-' {
                pos += 1;
            } else {
                break;
            }
        }

        let s = &self.data[self.pos..pos];
        let f = s.parse::<f32>().unwrap_or_default();

        self.pos = pos;
        self.skip_whitespace();
        return Ok(f);
    }

    fn parse_string(&mut self) -> Result<String, ParseError> {
        let c = self.data.chars().nth(self.pos).unwrap_or_default();
        if self.match_end() || c != '"' {
            return Err(ParseError::new("Expected <string>"));
        }

        let mut pos = self.pos + 1;
        loop {
            let c = self.data.chars().nth(pos).unwrap_or_default();
            if c == '"' {
                break;
            }
            pos += 1;
            if pos == self.data.len() {
                return Err(ParseError::new("Unterminated string constant"));
            }
        }

        let ret = self.data[self.pos + 1..pos].to_string();
        self.pos = pos + 1;

        self.skip_whitespace();

        return Ok(ret);
    }

    fn parse_float_triple(&mut self) -> Result<[f32; 3], ParseError> {
        let mut values = [0.0; 3];

        self.expect_literal("<")?;
        values[0] = self.parse_float()?;
        self.expect_literal(",")?;
        values[1] = self.parse_float()?;
        self.expect_literal(",")?;
        values[2] = self.parse_float()?;
        self.expect_literal(">")?;
        
        return Ok(values);
    }

    fn parse_color(&mut self) -> Result<Color, ParseError> {
        let values = self.parse_float_triple()?;
        return Ok(Color::new(values[0], values[1], values[2]));
    }

    fn parse_point(&mut self) -> Result<Point3, ParseError> {
        let values = self.parse_float_triple()?;
        return Ok(Point3::new(values[0], values[1], values[2]));
    }

    fn parse_radiance(&mut self) -> Result<Radiance, ParseError> {
        let values = self.parse_float_triple()?;
        return Ok(Radiance::new(values[0], values[1], values[2]));
    }

    fn parse_vector(&mut self) -> Result<Vec3, ParseError> {
        let values = self.parse_float_triple()?;
        return Ok(Vec3::new(values[0], values[1], values[2]));
    }

    fn parse_scene(&mut self) -> Result<Scene, ParseError> {
        let mut camera = None;
        let mut primitives = Vec::new();
        let mut lights = Vec::new();

        while !self.match_end() {
            if let Some(new_camera) = self.try_parse_camera()? {
                camera = Some(new_camera);
                continue;
            } else if let Some(primitive) = self.try_parse_primitive()? {
                primitives.push(primitive);
                continue;
            } else if let Some(light) = self.try_parse_light()? {
                lights.push(light);
                continue;
            } else {
                self.error_unexpected()?;
            }
        }

        return Ok(Scene::new(camera.expect("Camera expected"), primitives, lights));
    }

    fn try_parse_camera(&mut self) -> Result<Option<Camera>, ParseError> {
        if !self.match_literal("camera") {
            return Ok(None);
        }
        self.expect_left_brace()?;

        let position = self.parse_point()?;
        let look_at = self.parse_point()?;
        let focal_length = self.parse_float()?;
        let aperture_size = self.parse_float()?;

        self.expect_right_brace()?;

        return Ok(Some(Camera::new(position, (look_at - position).normalize(), Vec3::new(0.0, 1.0, 0.0), 60.0, focal_length, aperture_size)));
    }

    fn try_parse_light(&mut self) -> Result<Option<Box<dyn Light>>, ParseError> {
        if self.match_literal("point_light") {
            self.expect_left_brace()?;

            let position = self.parse_point()?;
            let radiance = self.parse_radiance()?;

            self.expect_right_brace()?;

            return Ok(Some(Box::new(object::light::Point::new(position, radiance))));
        } else if self.match_literal("sky") {
            self.expect_left_brace()?;

            let radiance = self.parse_radiance()?;

            self.expect_right_brace()?;

            return Ok(Some(Box::new(object::light::Sky::new(radiance))));
        } else {
            return Ok(None);
        }
    }

    fn try_parse_primitive(&mut self) -> Result<Option<Primitive>, ParseError> {
        let mut shape : Box<dyn Shape>;

        if self.match_literal("sphere") {
            self.expect_left_brace()?;

            let position = self.parse_point()?;
            let radius = self.parse_float()?;
            shape = Box::new(Sphere::new(position, radius));
        } else if self.match_literal("quad") {
            self.expect_left_brace()?;

            let position = self.parse_point()?;
            let side1 = self.parse_vector()?;
            let side2 = self.parse_vector()?;
            shape = Box::new(Quad::new(position, side1, side2));
        } else if self.match_literal("model") {
            self.expect_left_brace()?;

            let filename = self.parse_string()?;
            if filename.ends_with(".ply") {
                shape = PlyLoader::load(filename.as_str()).expect("Model expected");
            } else if filename.ends_with(".bpt") {
                shape = BptLoader::load(filename.as_str()).expect("Model expected");
            } else {
                return Err(ParseError::new(&format!("Invalid model filename {}", filename)));
            }
        } else {
            return Ok(None);
        }

        let mut surface = None;
        while !self.match_right_brace() {
            if let Some(new_surface) = self.try_parse_surface()? {
                surface = Some(new_surface);
                continue;
            } else if let Some(transformation) = self.try_parse_transformation()? {
                shape = Box::new(Transformed::new(shape, transformation));
                continue;
            } else {
                self.error_unexpected()?;
            }
        }
    
        return Ok(Some(Primitive::new(shape, surface.expect("Surface expected"))));
    }

    fn try_parse_surface(&mut self) -> Result<Option<Surface>, ParseError> {
        if !self.match_literal("surface") {
            return Ok(None);
        }
        self.expect_left_brace()?;

        let mut albedo = None;
        let mut brdfs = Vec::new();
        let mut transmit_ior = 0.0;
        let mut radiance = Radiance::ZERO;
        let mut normal_map = None;

        while !self.match_right_brace() {
            if let Some(new_albedo) = self.try_parse_albedo()? {
                albedo = Some(new_albedo);
                continue;
            } else if let Some((new_brdfs, new_transmit_ior)) = self.try_parse_brdfs()? {
                brdfs = new_brdfs;
                transmit_ior = new_transmit_ior;
                continue;
            } else if let Some(new_normal_map) = self.try_parse_normal_map()? {
                normal_map = Some(new_normal_map);
                continue;
            } else if self.match_literal("radiance") {
                radiance = self.parse_radiance()?;
                continue;
            } else {
                self.error_unexpected()?;
            }
        }

        return Ok(Some(Surface::new(albedo.expect("Albedo expected"), brdfs, transmit_ior, radiance, normal_map)));
    }

    fn try_parse_transformation(&mut self) -> Result<Option<Transformation>, ParseError> {
        if !self.match_literal("transform") {
            return Ok(None);
        }
        self.expect_left_brace()?;

        let mut result = Transformation::identity();
        while !self.match_right_brace() {
            if self.match_literal("translate") {
                let vector = self.parse_vector()?;
                result = result.transform(Transformation::translate(vector));
                continue;
            } else if self.match_literal("rotate") {
                let vector = self.parse_vector()?;
                result = result.transform(Transformation::rotate(vector));
                continue;
            } else if self.match_literal("scale") {
                let vector = self.parse_vector()?;
                result = result.transform(Transformation::scale(vector));
                continue;
            } else if self.match_literal("uniform_scale") {
                let scale = self.parse_float()?;
                result = result.transform(Transformation::uniform_scale(scale));
                continue;
            } else {
                self.error_unexpected()?;
            }
        }

        return Ok(Some(result));
    }

    fn try_parse_normal_map(&mut self) -> Result<Option<NormalMap>, ParseError> {
        if !self.match_literal("normal_map") {
            return Ok(None);
        }
        let filename = self.parse_string()?;
        let magnitude = self.parse_float()?;

        if let Ok(texture) = BmpLoader::load(filename.as_str()) {        
            return Ok(Some(NormalMap::new(texture, magnitude)));
        } else {
            return Ok(None);
        }
    }

    fn try_parse_albedo(&mut self) -> Result<Option<Box<dyn Albedo>>, ParseError> {
        if !self.match_literal("albedo") {
            return Ok(None);
        }
        self.expect_left_brace()?;

        let mut albedo: Option<Box<dyn Albedo>> = None;
        if self.match_literal("color") {
            let color = self.parse_color()?;
            albedo = Some(Box::new(Solid::new(color)));
        } else if self.match_literal("texture") {
            let filename = self.parse_string()?;
            if let Ok(texture) = BmpLoader::load(filename.as_str()) {
                albedo = Some(Box::new(Texture::new(texture)));
            }
        } else {
            self.error_unexpected()?;
        }

        self.expect_right_brace()?;
        return Ok(albedo);
    }

    fn try_parse_brdfs(&mut self) -> Result<Option<(Vec<Box<dyn Brdf>>, f32)>, ParseError> {
        if !self.match_literal("brdf") {
            return Ok(None);
        }
        self.expect_left_brace()?;

        let mut brdfs: Vec<Box<dyn Brdf>> = Vec::new();
        let mut transmit_ior = 0.0;
        while !self.match_right_brace() {
            if self.match_literal("lambert") {
                let strength = self.parse_float()?;
                brdfs.push(Box::new(Lambert::new(strength)));
                continue;
            } else if self.match_literal("phong") {
                let strength = self.parse_float()?;
                let power = self.parse_float()?;
                brdfs.push(Box::new(Phong::new(strength, power)));
                continue;
            } else if self.match_literal("oren_nayar") {
                let strength = self.parse_float()?;
                let roughness = self.parse_float()?;
                brdfs.push(Box::new(OrenNayar::new(strength, roughness)));
                continue;
            } else if self.match_literal("torrance_sparrow") {
                let strength = self.parse_float()?;
                let roughness = self.parse_float()?;
                let ior = self.parse_float()?;
                brdfs.push(Box::new(TorranceSparrow::new(strength, roughness, ior)));
                continue;
            } else if self.match_literal("transmit") {
                transmit_ior = self.parse_float()?;
                continue;
            } else {
                self.error_unexpected()?;
            }
        }

        return Ok(Some((brdfs, transmit_ior)));
    }
}