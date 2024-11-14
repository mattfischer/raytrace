use crate::geo;
use crate::object;

use geo::Beam;
use geo::Bivec2;
use geo::Normal3;
use geo::Vec2;

use object::Color;
use object::Primitive;
use object::ShapeIntersection;
use object::Scene;

pub struct Intersection<'a, 'b> {
    pub scene : &'a Scene,
    pub primitive : Option<&'a Primitive>,
    pub shape_isect : ShapeIntersection,
    pub beam : &'b Beam,
    pub normal : Normal3,
    pub facing_normal : Normal3,
    pub surface_projection : Bivec2,
    pub albedo : Color
}

impl<'a, 'b> Intersection<'a, 'b> {
    pub fn new(scene : &'a Scene, primitive : Option<&'a Primitive>, beam : &'b Beam, shape_isect : ShapeIntersection) -> Intersection<'a, 'b> {
        if let Some(prim) = primitive {
            let projection = beam.project(shape_isect.distance, shape_isect.normal);
            let vv = shape_isect.tangent.u % shape_isect.tangent.v;
            let v = vv / vv.mag2();
            let du = Vec2::new((projection.u % shape_isect.tangent.v) * v, (shape_isect.tangent.u % projection.u) * v);
            let dv = Vec2::new((projection.v % shape_isect.tangent.v) * v, (shape_isect.tangent.u % projection.v) * v);
            let surface_projection = Bivec2::new(du, dv);

            let normal = if let Some(normal_map) = &prim.surface.normal_map {
                normal_map.perturb_normal(shape_isect.surface_point, surface_projection, shape_isect.normal, shape_isect.tangent)
            } else {
                shape_isect.normal
            };
            
            let dir_out = -beam.ray.direction;
            let facing_normal = (normal.to_vec3() * dir_out).signum() * normal;

            let albedo = prim.surface.albedo.color(shape_isect.surface_point, surface_projection);
            
            return Intersection{scene, primitive, shape_isect, beam, normal, facing_normal, surface_projection, albedo};
        } else {
            return Intersection{scene, primitive, shape_isect, beam, normal: Normal3::ZERO, facing_normal: Normal3::ZERO, surface_projection: Bivec2::ZERO, albedo: Color::ZERO};
        }
    }
}