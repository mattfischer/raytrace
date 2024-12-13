use crate::geo;
use crate::object;

use geo::Beam;
use geo::Bivec2;
use geo::Normal3;
use geo::Point3;
use geo::Ray;
use geo::Vec2;

use object::Color;
use object::Primitive;
use object::Scene;
use object::ShapeIntersection;

#[derive(Copy, Clone)]
pub struct Intersection<'a> {
    pub scene: &'a Scene,
    pub primitive: &'a Primitive,
    pub shape_isect: ShapeIntersection,
    pub point: Point3,
    pub ray: Ray,
    pub normal: Normal3,
    pub facing_normal: Normal3,
    pub albedo: Color,
}

impl<'a> Intersection<'a> {
    pub fn new(
        scene: &'a Scene,
        primitive: &'a Primitive,
        beam: Beam,
        shape_isect: ShapeIntersection,
    ) -> Intersection<'a> {
        let projection = beam.project(shape_isect.distance, shape_isect.normal);
        let vv = shape_isect.tangent.u % shape_isect.tangent.v;
        let v = vv / vv.mag2();
        let du = Vec2::new(
            (projection.u % shape_isect.tangent.v) * v,
            (shape_isect.tangent.u % projection.u) * v,
        );
        let dv = Vec2::new(
            (projection.v % shape_isect.tangent.v) * v,
            (shape_isect.tangent.u % projection.v) * v,
        );
        let surface_projection = Bivec2::new(du, dv);
        let point = beam.ray.origin + beam.ray.direction * shape_isect.distance;

        let normal = if let Some(normal_map) = &primitive.surface.normal_map {
            normal_map.perturb_normal(
                shape_isect.surface_point,
                surface_projection,
                shape_isect.normal,
                shape_isect.tangent,
            )
        } else {
            shape_isect.normal
        };

        let ray = beam.ray;
        let dir_out = -ray.direction;
        let facing_normal = (normal * dir_out).signum() * normal;

        let albedo = primitive
            .surface
            .albedo
            .color(shape_isect.surface_point, surface_projection);

        return Intersection {
            scene,
            primitive,
            shape_isect,
            point,
            ray,
            normal,
            facing_normal,
            albedo,
        };
    }
}
