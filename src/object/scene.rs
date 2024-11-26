use crate::geo;
use crate::geo::Transformation;
use crate::object;

use geo::Beam;
use geo::Point3;

use object::BoundingVolume;
use object::BoundingVolumeHierarchy;
use object::Camera;
use object::Intersection;
use object::PointLight;
use object::Primitive;
use object::Radiance;
use object::ShapeIntersection;

#[derive(Debug)]
pub struct Scene {
    pub camera : object::Camera,
    pub primitives : Vec<Primitive>,
    pub point_lights : Vec<PointLight>,
    pub area_lights : Vec<usize>,
    pub sky_radiance : Radiance,
    bvh : BoundingVolumeHierarchy
}

impl Scene {
    pub fn new(camera : Camera, primitives : Vec<Primitive>, point_lights : Vec<PointLight>, sky_radiance : Radiance) -> Scene {
        let mut centroids = Vec::new();
        let mut area_lights = Vec::new();
        let xform = Transformation::identity();
        for (idx, primitive) in primitives.iter().enumerate() {
            centroids.push(primitive.shape.bounding_volume(xform).centroid());
            if primitive.surface.radiance.mag2() > 0.0 {
                area_lights.push(idx);
            }
        }

        let func = |idx : usize| -> BoundingVolume {
            return primitives[idx].shape.bounding_volume(xform);
        };
        let bvh = BoundingVolumeHierarchy::with_volumes(&centroids[..], &func);

        return Scene {camera, primitives, point_lights, area_lights, sky_radiance, bvh}
    }

    pub fn intersect<'a, 'b>(&'a self, beam : &'b Beam, max_distance : f32, closest : bool) -> Option<Intersection<'a, 'b>> {
        let mut isect = None;

        let mut func = |index : usize, max_distance : &mut f32| {
            let primitive = &self.primitives[index];
            if let Some(shape_isect) = primitive.shape.intersect(beam.ray, *max_distance, closest) {
                *max_distance = shape_isect.distance;
                isect = Some(Intersection::new(self, primitive, beam, shape_isect));
                return true;
            } else {
                return false;
            }
        };

        let raydata = BoundingVolume::get_raydata(beam.ray);
        let mut distance = max_distance;
        self.bvh.intersect(raydata, &mut distance, closest, &mut func);

        return isect;
    }
}