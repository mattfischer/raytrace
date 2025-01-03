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
    pub primitive_idx: usize,
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
        primitive_idx: usize,
        beam: Beam,
        shape_isect: ShapeIntersection,
    ) -> Intersection<'a> {
        let projection = beam.project(shape_isect.distance, shape_isect.normal);
        let primitive = &scene.primitives[primitive_idx];
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
            primitive_idx,
            shape_isect,
            point,
            ray,
            normal,
            facing_normal,
            albedo,
        };
    }

    pub fn with_flat(flat: FlatIntersection, scene: &'a Scene) -> Intersection<'a> {
        return Intersection {
            scene: scene,
            primitive: &scene.primitives[flat.primitive_idx],
            primitive_idx: flat.primitive_idx,
            shape_isect: flat.shape_isect,
            point: flat.point,
            ray: flat.ray,
            normal: flat.normal,
            facing_normal: flat.facing_normal,
            albedo: flat.albedo,
        };
    }
}

#[derive(Clone, Copy, Default)]
pub struct FlatIntersection {
    pub primitive_idx: usize,
    pub shape_isect: ShapeIntersection,
    pub point: Point3,
    pub ray: Ray,
    pub normal: Normal3,
    pub facing_normal: Normal3,
    pub albedo: Color,
}

impl<'a> From<Intersection<'a>> for FlatIntersection {
    fn from(value: Intersection<'a>) -> Self {
        return FlatIntersection {
            primitive_idx: value.primitive_idx,
            shape_isect: value.shape_isect,
            point: value.point,
            ray: value.ray,
            normal: value.normal,
            facing_normal: value.facing_normal,
            albedo: value.albedo,
        };
    }
}
