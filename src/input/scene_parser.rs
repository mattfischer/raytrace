use crate::geo;
use crate::input;
use crate::object;

use input::TextureLoader;
use input::ModelLoader;

use chumsky::prelude::*;

pub struct SceneParser;

impl SceneParser {
    fn make_parser() -> impl Parser<char, object::Scene, Error = Simple<char>> {    
        let float =
            just('-').or_not()
            .chain::<char, _, _>(text::int(10))
            .chain::<char, _, _>(
                just('.')
                .chain(text::digits(10))
                .or_not()
            )
            .collect::<String>()
            .from_str::<f32>()
            .unwrapped();

        let triple =
            float.clone()
            .separated_by(just(',')).exactly(3)
            .delimited_by(just('<'), just('>'))
            .padded();

        let vector =
            triple.clone()
            .map(|v| geo::Vec3::new(v[0], v[1], v[2]));

        let point =
            triple.clone()
            .map(|v| geo::Point3::new(v[0], v[1], v[2]));

        let color =
            triple.clone()
            .map(|v| object::Color::new(v[0], v[1], v[2]));

        let radiance =
            triple.clone()
            .map(|v| object::Radiance::new(v[0], v[1], v[2]));

        let string =
            filter(|c| *c != '"').repeated()
            .delimited_by(just('"'), just('"'))
            .padded()
            .collect::<String>();
        
        let transform_item =
            choice((
                text::keyword("translate")
                .ignore_then(vector.clone())
                .map(|v| geo::Transformation::translate(v))
            ,
                text::keyword("rotate")
                .ignore_then(vector.clone())
                .map(|v| geo::Transformation::rotate(v))
            ,
                text::keyword("scale")
                .ignore_then(vector.clone())
                .map(|v| geo::Transformation::scale(v))
            ,
                text::keyword("uniform_scale")
                .ignore_then(float.clone())
                .map(|f| geo::Transformation::uniform_scale(f))
            ));

        let transform =
            text::keyword("transform")
            .ignore_then(
                transform_item
                .repeated()
                .delimited_by(just('{'), just('}'))
            ).map(|transforms| {
                let mut result = transforms[0];
                for t in transforms[1..].iter() {
                    result = result.transform(*t);
                }
                return result;
            });

        let albedo =
            choice((
                text::keyword("color")
                .ignore_then(color.clone())
                .map(|color| -> Option<Box<dyn object::Albedo>> {
                    return Some(Box::new(object::albedo::Solid::new(color)));
                })
            ,
                text::keyword("texture")
                .ignore_then(string.clone())
                .map(|filename| -> Option<Box<dyn object::Albedo>> {
                    if let Some(texture) = TextureLoader::load(filename) {
                        return Some(Box::new(object::albedo::Texture::new(texture)));
                    } else {
                        return None;
                    }
                })
            ));

        let brdf_item =
            choice((
                text::keyword("lambert")
                .ignore_then(float.clone())
                .map(|strength| -> Box<dyn object::Brdf> {
                    Box::new(object::brdf::Lambert::new(strength))
                })
            ,
                text::keyword("phong")
                .ignore_then(float.clone())
                .then(float.clone())
                .map(|(strength, power)| -> Box<dyn object::Brdf> {
                    Box::new(object::brdf::Phong::new(strength, power))
                })
            ,
                text::keyword("oren_nayar")
                .ignore_then(float.clone())
                .then(float.clone())
                .map(|(strength, roughness)| -> Box<dyn object::Brdf> {
                    Box::new(object::brdf::OrenNayar::new(strength, roughness))
                })
            ,
                text::keyword("torrance_sparrow")
                .ignore_then(float.clone())
                .then(float.clone())
                .then(float.clone())
                .map(|((strength, roughness), ior)| -> Box<dyn object::Brdf> {
                    Box::new(object::brdf::TorranceSparrow::new(strength, roughness, ior))
                })
            ));

        enum SurfaceItem {
            Albedo(Option<Box<dyn object::Albedo>>),
            Brdfs(Vec<Box<dyn object::Brdf>>),
            Radiance(object::Radiance),
            NormalMap(Option<object::NormalMap>),
            TransmitIor(f32)
        }

        let surface_item =
            choice((
                text::keyword("albedo")
                .ignore_then(
                    albedo
                    .delimited_by(just('{'), just('}'))
                ).map(|albedo| SurfaceItem::Albedo(albedo))
            ,
                text::keyword("brdf")
                .ignore_then(
                    brdf_item
                    .repeated()
                    .delimited_by(just('{'), just('}'))
                ).map(|brdfs| SurfaceItem::Brdfs(brdfs))
            ,
                text::keyword("radiance")
                .ignore_then(radiance.clone())
                .map(|radiance| SurfaceItem::Radiance(radiance))
            ,
                text::keyword("normal_map")
                .ignore_then(string.clone())
                .then(float.clone())
                .map(|(filename, magnitude)| {
                    if let Some(texture) = TextureLoader::load(filename) {
                        let normal_map = object::NormalMap::new(texture, magnitude);
                        return SurfaceItem::NormalMap(Some(normal_map));    
                    } else {
                        return SurfaceItem::NormalMap(None);
                    }
                })
            ,
                text::keyword("transmit_ior")
                .ignore_then(float.clone())
                .map(|transmit_ior| SurfaceItem::TransmitIor(transmit_ior))
            ));
 
        let surface =
            text::keyword("surface")
            .ignore_then(
                surface_item
                .repeated()
                .delimited_by(just('{'), just('}'))
            ).map(|items| {
                let mut albedo : Option<Box<dyn object::Albedo>> = None;
                let mut brdfs = Vec::new();
                let mut transmit_ior = 1.0;
                let mut normal_map : Option<object::NormalMap> = None;
                let mut radiance = object::Radiance::ZERO;

                for item in items {
                    match item {
                        SurfaceItem::Albedo(a) => albedo = a,
                        SurfaceItem::Brdfs(b) => brdfs = b,
                        SurfaceItem::TransmitIor(i) => transmit_ior = i,
                        SurfaceItem::NormalMap(n) => normal_map = n,
                        SurfaceItem::Radiance(r) => radiance = r
                    }
                }
                
                if let Some(albedo) = albedo {
                    let surface = object::Surface::new(albedo, brdfs, transmit_ior, radiance, normal_map);
                    return Some(surface);
                } else {
                    return None;
                }
            }
        );

        enum PrimitiveModifier {
            Transform(geo::Transformation),
            Surface(Option<object::Surface>)
        }

        let primitive_modifiers =
            choice((
                transform.map(|t| PrimitiveModifier::Transform(t))
            ,
                surface.map(|s| PrimitiveModifier::Surface(s))
            )).repeated();

        let sphere =
            text::keyword("sphere")
            .ignore_then(
                point.clone()
                .then(float.clone())
                .then(primitive_modifiers.clone())
                .delimited_by(just('{'), just('}'))
            ).map(|((center, radius), modifiers)| {
                let sphere: Box<dyn object::Shape> = Box::new(object::shape::Sphere::new(center, radius));
                return (Some(sphere), modifiers);
            });

        let quad =
            text::keyword("quad")
            .ignore_then(
                point.clone()
                .then(vector.clone())
                .then(vector.clone())
                .then(primitive_modifiers.clone())
                .delimited_by(just('{'), just('}'))
            ).map(|(((position, side1), side2), modifiers)| {
                let quad: Box<dyn object::Shape> = Box::new(object::shape::Quad::new(position, side1, side2));
                return (Some(quad), modifiers);
            });

        let model =
            text::keyword("model")
            .ignore_then(
                string.clone()
                .then(primitive_modifiers.clone())
                .delimited_by(just('{'), just('}'))
            ).map(|(filename, modifiers)| {
                let model = ModelLoader::load(filename);
                return (model, modifiers);
            });

        let primitive =
            choice((sphere, quad, model))
            .map(|(shape, modifiers)| {
                let mut primitive = None;
                let mut surface = None;
                if let Some(shape) = shape {
                    let mut shape = shape;
                    for modifier in modifiers {
                        match modifier {
                            PrimitiveModifier::Surface(s) => surface = s,
                            PrimitiveModifier::Transform(t) => shape = Box::new(object::shape::Transformed::new(shape, t))
                        }
                    }

                    if let Some(s) = surface {
                        primitive = Some(object::Primitive::new(shape, s));
                    }
                }

                return primitive;
            });

        let point_light =
            text::keyword("point_light")
            .ignore_then(
                point.clone()
                .then(radiance.clone())
            ).delimited_by(just('{'), just('}'))
            .map(|(position, radiance)| {
                return object::PointLight::new(position, radiance);
            });

        let sky =
            text::keyword("sky")
            .ignore_then(
                radiance.clone()
                .delimited_by(just('{'), just('}'))
            );

        let camera =
            text::keyword("camera")
            .ignore_then(
                vector.clone().map(|v| geo::Point3::from_vec3(v))
                .then(vector.clone().map(|v| geo::Point3::from_vec3(v)))
                .then(float.clone())
                .then(float.clone())
                )
            .delimited_by(just('{'), just('}'))
            .map(|(((position, look_at), focal_length), aperture_size)| {
                return object::Camera::new(position, (look_at - position).normalize(), geo::Vec3::new(0.0, 1.0, 0.0), 60.0, focal_length, aperture_size);
            });

        enum Object {
            Primitive(Option<object::Primitive>),
            PointLight(object::PointLight),
            Sky(object::Radiance),
            Camera(object::Camera)
        }

        let object =
            choice((
                primitive.map(|primitive| Object::Primitive(primitive))
            ,
                point_light.map(|point_light| Object::PointLight(point_light))
            ,
                sky.map(|radiance| Object::Sky(radiance))
            ,
                camera.map(|camera| Object::Camera(camera))
            ));

        let scene =
            object
            .repeated()
            .then_ignore(end())
            .map(|objects| {
                let mut primitives = Vec::new();
                let mut camera = object::Camera::new(geo::Point3::ZERO, geo::Vec3::new(0.0, 0.0, 1.0), geo::Vec3::new(0.0, 1.0, 0.0), 60.0, 1.0, 1.0);
                let mut point_lights = Vec::new();
                let mut sky_radiance = object::Radiance::ZERO;

                for object in objects {
                    match object {
                        Object::Primitive(Some(p)) => primitives.push(p),
                        Object::Primitive(None) => (),
                        Object::Camera(c) => camera = c,
                        Object::PointLight(p) => point_lights.push(p),
                        Object::Sky(r) => sky_radiance = r,
                    }
                }
                return object::Scene::new(camera, primitives, point_lights, sky_radiance);
            });

        return scene;
    }

    pub fn parse_scene(filename : String) -> Option<object::Scene> {
        let result = std::fs::read_to_string(filename);
        if let Ok(contents) = result {
            let parser = Self::make_parser();
            let scene = parser.parse(contents);
            if let Ok(s) = scene {
                return Some(s);
            }
        }

        return None;
    }
}