use crate::geo;
use crate::geo::Transformation;
use crate::object;

use geo::Beam;
use geo::Point3;

use object::BoundingVolume;
use object::BoundingVolumeHierarchy;
use object::Camera;
use object::Intersection;
use object::Primitive;
use object::ShapeIntersection;

#[derive(Debug)]
pub struct Scene {
    camera : object::Camera,
    primitives : Vec<Primitive>,
    bvh : BoundingVolumeHierarchy
}

impl Scene {
    pub fn new(camera : Camera, primitives : Vec<Primitive>) -> Scene {
        let mut centroids = Vec::<Point3>::new();
        let xform = Transformation::identity();
        for primitive in primitives.iter() {
            centroids.push(primitive.shape.bounding_volume(xform).centroid())
        }

        let func = |idx : usize| -> BoundingVolume {
            return primitives[idx].shape.bounding_volume(xform);
        };
        let bvh = BoundingVolumeHierarchy::with_volumes(&centroids[..], &func);

        return Scene {camera, primitives, bvh}
    }

    pub fn intersect<'a, 'b>(&'a self, beam : &'b Beam, max_distance : f32, closest : bool) -> Intersection<'a, 'b> {
        let mut shape_isect = ShapeIntersection::new();
        let mut primitive : Option<&Primitive> = None;

        shape_isect.distance = max_distance;

        let mut func = |index : usize, max_distance : &mut f32| {
            let p = &self.primitives[index];
            if p.shape.intersect(&beam.ray, &mut shape_isect, closest) {
                primitive = Some(p);
                *max_distance = shape_isect.distance;
                return true;
            } else {
                return false;
            }
        };

        let raydata = BoundingVolume::get_raydata(beam.ray);
        let mut distance = max_distance;
        self.bvh.intersect(raydata, &mut distance, closest, &mut func);

        return Intersection::new(self, primitive, beam, shape_isect);
    }
}