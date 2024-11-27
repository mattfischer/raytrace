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

        let mut func = |index : usize, max_distance : f32| {
            let primitive = &self.primitives[index];
            if let Some(shape_isect) = primitive.shape.intersect(beam.ray, max_distance, closest) {
                isect = Some(Intersection::new(self, primitive, beam, shape_isect));
                return Some(shape_isect.distance);
            } else {
                return None;
            }
        };

        let raydata = BoundingVolume::get_raydata(beam.ray);
        self.bvh.intersect(raydata, max_distance, closest, &mut func);

        return isect;
    }
}