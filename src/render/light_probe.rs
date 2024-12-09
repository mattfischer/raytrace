use core::f32;

use crate::geo;
use crate::object;
use crate::render;

use geo::Beam;
use geo::Bivec3;
use geo::OrthonormalBasis;
use geo::Ray;
use geo::Vec3;

use object::Color;
use object::Intersection;
use object::Radiance;
use object::sampler::Random;

use render::lighter::UniPath;
use render::Lighter;
use render::Framebuffer;

pub struct LightProbe<'a, 'b, 'c> {
    isect : &'a Intersection<'b, 'c>,
    basis : OrthonormalBasis,
    sampler : Random,
    lighter : UniPath
}

impl<'a, 'b, 'c> LightProbe<'a, 'b, 'c> {
    pub fn new(isect: &'a Intersection<'b, 'c>) -> LightProbe<'a, 'b, 'c> {
        let basis = OrthonormalBasis::new(isect.facing_normal.into());
        let sampler = Random::new();
        let lighter = UniPath::new();
        return LightProbe {isect, basis, sampler, lighter};
    }

    pub fn get_sample(&mut self) -> (f32, f32, Color) {
        let surface = &self.isect.primitive.surface;
        let nrm_facing = self.isect.facing_normal;

        let (_, dir_in, _) = surface.sample(self.isect, &mut self.sampler);
        let dot = dir_in * nrm_facing;
        let pnt_offset = self.isect.point + Vec3::from(nrm_facing) * 0.01;
        let ray = Ray::new(pnt_offset, dir_in);
        let beam = Beam::new(ray, Bivec3::ZERO, Bivec3::ZERO);
       
        let isect2 = self.isect.scene.intersect(&beam, f32::MAX, true);
        let irad = match isect2 {
            Some(isect2) => self.lighter.light(&isect2, &mut self.sampler) * dot,
            None => Radiance::ZERO
        };
        
		let dir_in_local = self.basis.world_to_local(dir_in);
        let azimuth = dir_in_local.y.atan2(dir_in_local.x);
        let elevation = dir_in_local.z.asin();
        let color = Framebuffer::tone_map(irad);

        return (azimuth, elevation, color);
    }
}