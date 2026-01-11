use crate::geo;
use crate::geo::Transformation;
use crate::object;

use geo::Beam;

use object::BoundingVolume;
use object::BoundingVolumeHierarchy;
use object::Camera;
use object::Intersection;
use object::Light;
use object::Primitive;

use std::sync::Arc;

pub struct Scene {
    pub camera: Camera,
    pub primitives: Vec<Primitive>,
    pub lights: Vec<Arc<Box<dyn Light>>>,
    pub sky_lights: Vec<Arc<Box<dyn Light>>>,
    pub area_lights: Vec<usize>,
    bvh: BoundingVolumeHierarchy,
}

impl Scene {
    pub fn new(
        camera: Camera,
        primitives: Vec<Primitive>,
        lights: Vec<Box<dyn Light>>
    ) -> Scene {
        let mut centroids = Vec::new();
        let mut area_lights = Vec::new();
        let xform = Transformation::identity();

        let mut all_lights = Vec::new();
        let mut sky_lights = Vec::new();
        for light in lights {
            let light = Arc::new(light);

            if light.is_sky() {
                sky_lights.push(light.clone());
            }

            all_lights.push(light);
        }

        for (idx, primitive) in primitives.iter().enumerate() {
            centroids.push(primitive.shape.bounding_volume(&xform).centroid());
            if let Some(light) = &primitive.light {
                area_lights.push(idx);
                all_lights.push(light.clone());
            }
        }

        let func = |idx: usize| -> BoundingVolume {
            return primitives[idx].shape.bounding_volume(&xform);
        };
        let bvh = BoundingVolumeHierarchy::from_volumes(&centroids[..], &func);

        return Scene {
            camera,
            primitives,
            lights: all_lights,
            area_lights,
            sky_lights,
            bvh,
        };
    }

    pub fn intersect<'a>(
        &'a self,
        beam: Beam,
        max_distance: f32,
        closest: bool,
    ) -> Option<Intersection<'a>> {
        let mut isect = None;

        let mut func = |index: usize, max_distance: f32| {
            let primitive = &self.primitives[index];
            if let Some(shape_isect) = primitive.shape.intersect(beam.ray, max_distance, closest) {
                isect = Some(Intersection::new(self, index, beam, shape_isect));
                return Some(shape_isect.distance);
            } else {
                return None;
            }
        };

        let raydata = BoundingVolume::get_raydata(beam.ray);
        self.bvh
            .intersect(raydata, max_distance, closest, &mut func);

        return isect;
    }
}
