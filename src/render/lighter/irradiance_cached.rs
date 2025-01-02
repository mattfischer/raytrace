use crate::geo;
use crate::object;
use crate::render;

use geo::Beam;
use geo::Bivec3;
use geo::Normal3;
use geo::OrthonormalBasis;
use geo::Point2;
use geo::Point3;
use geo::Ray;
use geo::Vec3;

use object::Color;
use object::Radiance;
use object::Sampler;
use object::Scene;
use object::sampler::Random;

use render::ExecutorJob;
use render::Framebuffer;
use render::Lighter;
use render::RasterJob;

use render::lighter::Direct;
use render::lighter::UniPath;

use core::f32;
use std::sync::Arc;
use std::sync::Mutex;
use std::sync::RwLock;

use std::f32::consts::PI;

#[derive(Clone, Copy, Default)]
struct RadianceGradient {
    red : Vec3,
    green : Vec3,
    blue : Vec3
}

impl RadianceGradient {
    pub const ZERO: RadianceGradient = RadianceGradient {red: Vec3::ZERO, green: Vec3::ZERO, blue: Vec3::ZERO};

    fn with_colors(red: Vec3, green: Vec3, blue: Vec3) -> Self {
        Self {red, green, blue}
    }

    fn with_radiance(radiance: Radiance, vector: Vec3) -> Self {
        Self {red: radiance.red * vector, green: radiance.green * vector, blue: radiance.blue * vector}
    }
}

impl std::ops::Add<RadianceGradient> for RadianceGradient {
    type Output = RadianceGradient;

    fn add(self, rhs: RadianceGradient) -> Self::Output {
        Self {red: self.red + rhs.red, green: self.green + rhs.green, blue: self.blue + rhs.blue}
    }
}

impl std::ops::AddAssign<RadianceGradient> for RadianceGradient {
    fn add_assign(&mut self, rhs: RadianceGradient) {
        *self = *self + rhs;
    }
}

impl std::ops::Mul<f32> for RadianceGradient {
    type Output = RadianceGradient;

    fn mul(self, rhs: f32) -> Self::Output {
        Self {red: self.red * rhs, green: self.green * rhs, blue: self.blue * rhs}
    }
}

impl std::ops::Div<f32> for RadianceGradient {
    type Output = RadianceGradient;

    fn div(self, rhs: f32) -> Self::Output {
        Self {red: self.red / rhs, green: self.green / rhs, blue: self.blue / rhs}
    }
}

impl std::ops::Mul<Vec3> for RadianceGradient {
    type Output = Radiance;

    fn mul(self, rhs: Vec3) -> Self::Output {
        Radiance {red: self.red * rhs, green: self.green * rhs, blue: self.blue * rhs}
    }
}

struct CacheEntry {
    point: Point3,
    normal: Normal3,
    radius: f32,
    radiance: Radiance,
    rot_grad: RadianceGradient,
    trans_grad: RadianceGradient
}

struct OctreeNode {
    entries: Vec<CacheEntry>,
    children: [Option<Box<OctreeNode>>; 8]
}

struct Octree {
    root: Option<Box<OctreeNode>>,
    origin: Point3,
    size: f32,
}

struct Cache {
    octree: RwLock<Octree>,
    threshold: f32
}

impl Cache {
    fn new(threshold: f32) -> Cache {
        let octree = RwLock::new(Octree {root: None, origin: Point3::ZERO, size: 0.0});

        return Cache {octree, threshold};
    }

    fn add(&self, entry: CacheEntry) {
        if let Ok(mut octree) = self.octree.write() {
            let R = entry.radius * self.threshold;
            if octree.root.is_none() {
                octree.root = Some(Box::new(OctreeNode {entries: Vec::new(), children: [const { None }; 8]}));
                octree.size = R;
                octree.origin = entry.point;
            }

            while 
                (entry.point.x - octree.origin.x).abs() > octree.size ||
                (entry.point.y - octree.origin.y).abs() > octree.size ||
                (entry.point.z - octree.origin.z).abs() > octree.size ||
                octree.size < R {
                let x = (entry.point.x - octree.origin.x).signum();
                let y = (entry.point.y - octree.origin.y).signum();
                let z = (entry.point.z - octree.origin.z).signum();
                
                let idx = if x < 0.0 { 1 } else { 0 } + if y < 0.0 { 2 } else { 0 } + if z < 0.0 { 4 } else { 0 };
                let mut new_root = Box::new(OctreeNode {entries: Vec::new(), children: [const { None }; 8]});
                new_root.children[idx] = octree.root.take();
                octree.root = Some(new_root);
                let size = octree.size;
                octree.origin += Vec3::new(x, y, z) * size;
                octree.size *= 2.0;
            }
    
            let mut origin = octree.origin;
            let mut size = octree.size;
            let mut node = octree.root.as_mut().unwrap();
            while size > R * 2.0 {
                let x = (entry.point.x - origin.x).signum();
                let y = (entry.point.y - origin.y).signum();
                let z = (entry.point.z - origin.z).signum();
                
                let idx = if x > 0.0 { 1 } else { 0 } + if y > 0.0 { 2 } else { 0 } + if z > 0.0 { 4 } else { 0 };
                let new_origin = origin + Vec3::new(x, y, z) * size / 2.0;
                if node.children[idx].is_none() {
                    node.children[idx] = Some(Box::new(OctreeNode {entries: Vec::new(), children: [const { None }; 8]}));
                }

                origin = new_origin;
                size /= 2.0;
                node = node.children[idx].as_mut().unwrap();
            }

            node.entries.push(entry);
        }
    }

    fn weight(entry: &CacheEntry, point: Point3, normal: Normal3) -> f32 {
        //return std::pow(std::max(double(0), 1.0f - (point - entry.point).magnitude2() / (1.0 * entry.radius * std::pow(normal * entry.normal, 4.0f))), 2);
        return 1.0 / ((point - entry.point).mag() / entry.radius + (1.0 - (normal * entry.normal).min(1.0)).sqrt());
    }

    fn is_entry_valid(entry: &CacheEntry, point: Point3, normal: Normal3, weight: f32, threshold: f32) -> bool {
        let d = (point - entry.point) * ((normal + entry.normal) / 2.0);
        return d >= -0.01 && weight > 1.0 / threshold;
    }

    fn distance2_to_node(point: Point3, idx: usize, origin: Point3, size: f32) -> f32 {
        let mut distance2 = 0.0;
        let mut d;

        if point.x < origin.x - size {
            d = (origin.x - size) - point.x;
            distance2 += d * d;
        }

        if point.y < origin.y - size {
            d = (origin.y - size) - point.y;
            distance2 += d * d;
        }

        if point.z < origin.z - size {
            d = (origin.z - size) - point.z;
            distance2 += d * d;
        }

        if point.x > origin.x + size {
            d = point.x - (origin.x + size);
            distance2 += d * d;
        }

        if point.y > origin.y + size {
            d = point.y - (origin.y + size);
            distance2 += d * d;
        }

        if point.z > origin.z + size {
            d = point.z - (origin.z + size);
            distance2 += d * d;
        }

        return distance2;
    }

    fn get_child_node(origin: Point3, size: f32, idx: usize) -> (Point3, f32) {
        let x = if (idx & 1 == 0) { -1.0 } else { 1.0 };
        let y = if (idx & 2 == 0) { -1.0 } else { 1.0 };
        let z = if (idx & 4 == 0) { -1.0 } else { 1.0 };
        
        let child_size = size / 2.0;
        return (origin + Vec3::new(x, y, z) * child_size, child_size);
    }

    fn visit_octree_node<'a, F>(node: &'a Option<Box<OctreeNode>>, origin: Point3, size: f32, point: Point3, callback: &mut F) -> bool 
    where F: FnMut(&'a CacheEntry) -> bool {
        if let Some(node) = node {
            for entry in node.entries.iter() {
                if !callback(entry) {
                    return false;
                }
            }

            for i in 0..8 {
                let (child_origin, child_size) = Self::get_child_node(origin, size, i);
                let distance2 = Self::distance2_to_node(point, i, child_origin, child_size);
                if distance2 < child_size * child_size {
                    if !Self::visit_octree_node(&node.children[i], child_origin, child_size, point, callback) {
                        return false;
                    }
                }
            }
        }

        return true;
    }
    
    pub fn test(&self, point: Point3, normal: Normal3) -> bool {
        let mut ret = false;

        if let Ok(octree) = self.octree.read() {
            let mut callback = |entry| {
                let w = Self::weight(entry, point, normal);
                if Self::is_entry_valid(entry, point, normal, w, self.threshold) {
                    ret = true;
                    return false;
                }
                return true;
            };

            Self::visit_octree_node(&octree.root, octree.origin, octree.size, point, &mut callback);
        }

        return ret;
    }

    fn interpolate(&self, point: Point3, normal: Normal3) -> Radiance {
        let mut result = Radiance::ZERO;

        if let Ok(octree) = self.octree.read() {
            let mut total_weight = 0.0;
            let mut irradiance = Radiance::ZERO;
            let mut threshold = self.threshold;

            for _ in 0..3 {
                let mut callback = |entry| {
                    let w = Self::weight(entry, point, normal);
                    if Self::is_entry_valid(entry, point, normal, w, threshold) {
                        if w.is_infinite() {
                            irradiance = entry.radiance;
                            total_weight = 1.0;
                            return false;
                        } else {
                            let cross = Vec3::from(normal % entry.normal);
                            let dist = point - entry.point;
                            irradiance += (entry.radiance + entry.rot_grad * cross + entry.trans_grad * dist) * w;
                            total_weight += w;
                        }
                    }
                    return true;
                };

                Self::visit_octree_node(&octree.root, octree.origin, octree.size, point, &mut callback);

                if total_weight > 0.0 {
                    irradiance = irradiance / total_weight;
                    break;
                } else {
                    threshold *= 2.0;
                }
            }

            result = irradiance.clamp();
        }

        return result;
    }
}

pub struct IrradianceCachedSettings {
    pub indirect_samples: usize,
    pub cache_threshold: f32
}

struct Inner {
    settings: IrradianceCachedSettings,
    cache: Cache,
    direct_lighter: Direct,
    unipath_lighter: UniPath
}

impl Inner {
    fn prerender_pixel(&self, x: usize, y: usize, framebuffer: &Mutex<Framebuffer>, scene: &Scene, sampler: &mut dyn Sampler) {
        let width;
        let height;
        if let Ok(framebuffer) = framebuffer.lock() {
            width = framebuffer.width;
            height = framebuffer.height;
        } else {
            return;
        }

        sampler.start_sample_with_xys(x, y, 0);
        
        let mut pixel_color = Color::ZERO;
        let beam = scene.camera.create_pixel_beam(Point2::new(x as f32, y as f32), width, height, Point2::ZERO);
        if let Some(isect) = scene.intersect(beam, f32::MAX, true) {
            let surface = &isect.primitive.surface;
            if surface.lambert > 0.0 {
                let pnt = isect.point;
                let nrm_facing = isect.facing_normal;

                if !self.cache.test(pnt, nrm_facing) {
                    let basis = OrthonormalBasis::new(nrm_facing.into());
                    let mut mean = 0.0;
                    let mut den = 0;
                    let mut rad = Radiance::ZERO;
                    let M = (self.settings.indirect_samples as f32).sqrt() as usize;
                    let N = self.settings.indirect_samples / M;
                    let mut samples = vec![Radiance::ZERO; M*N];
                    let mut sample_distances = vec![0.0; M*N];
                    for k in 0..N {
                        for j in 0..M {
                            sampler.start_sample_with_index(0);

                            let phi = 2.0 * PI * (k as f32 + sampler.get_value()) / (N as f32);
                            let theta = (j as f32 + sampler.get_value()).sqrt().asin() / (M as f32);
                            let dir_in = basis.local_to_world(Vec3::with_spherical(phi, theta, 1.0));

                            let pnt_offset = pnt + Vec3::from(nrm_facing) * 0.01;
                            let ray = Ray::new(pnt_offset, dir_in);
                            let beam = Beam::new(ray, Bivec3::ZERO, Bivec3::ZERO);
                            
                            if let Some(isect2) = scene.intersect(beam, f32::MAX, true) {
                                mean += 1.0 / isect2.shape_isect.distance;
                                den += 1;
                                let mut rad2 = self.unipath_lighter.light(&isect2, sampler);
                                rad2 = rad2 - isect2.primitive.surface.radiance;

                                samples[k*M + j] = rad2;
                                sample_distances[k*M + j] = isect2.shape_isect.distance;

                                rad += rad2 * PI / ((M * N) as f32);
                            } else {
                                sample_distances[k*M + j] = f32::MAX;
                            }
                        }
                    }

                    if mean > 0.0 {
                        let projected_pixel_size = scene.camera.project_size(2.0 / (width as f32), isect.shape_isect.distance);
                        let min_radius = 3.0 * projected_pixel_size / self.cache.threshold;
                        let max_radius = 20.0 * min_radius;
                        let radius = (den as f32) / mean;
                        let entry_radius = radius.min(max_radius).max(min_radius);
                        
                        let mut trans_grad = RadianceGradient::ZERO;
                        let mut rot_grad = RadianceGradient::ZERO;

                        for k in 0..N {
                            let k1 = if k > 0 { k - 1 } else { N - 1 };
                            let phi = 2.0 * PI * (k as f32) / (N as f32);
                            let u = basis.local_to_world(Vec3::with_spherical(phi, 0.0, 1.0));
                            let v = basis.local_to_world(Vec3::with_spherical(phi + PI / 2.0, 0.0, 1.0));

                            for j in 0..M {
                                let theta_minus = ((j as f32) / (M as f32)).sqrt().asin();
                                let theta_plus = (((j + 1) as f32) / (M as f32)).sqrt().asin();
                    
                                if j > 0 {
                                    let j1 = j - 1;
                                    let c = u * theta_minus.sin() * theta_minus.cos() * theta_minus.cos() * 2.0 * PI / ((N as f32) * sample_distances[k*M + j].min(sample_distances[k*M + j1]));
                                    trans_grad += RadianceGradient::with_radiance(samples[k*M + j] - samples[k*M + j1], c);
                                }

                                let c = v * (theta_plus.sin() - theta_minus.sin()) / (sample_distances[k*M + j].min(sample_distances[k1*M + j]));
                                trans_grad += RadianceGradient::with_radiance(samples[k*M + j] - samples[k1*M + j], c);
                                rot_grad += RadianceGradient::with_radiance(samples[k*M + j], v) * theta_minus.tan() * PI / ((M*N) as f32);
                            }
                        }

                        if radius < min_radius {
                            trans_grad = trans_grad * radius / min_radius;
                        }

                        let new_entry = CacheEntry {point: pnt, normal: nrm_facing, radiance: rad, radius: entry_radius, rot_grad, trans_grad};
                        self.cache.add(new_entry);
                        pixel_color = Color::ONE;
                    }
                }
            }
        }

        if let Ok(mut framebuffer) = framebuffer.lock() {
            framebuffer.set_pixel(x, y, pixel_color);
        }
    }
}
pub struct IrradianceCached {
    inner: Arc<Inner>
}

struct ThreadLocal {
    sampler: Random
}

impl IrradianceCached {
    pub fn new(settings: IrradianceCachedSettings) -> IrradianceCached {
        let direct_lighter = Direct::new();
        let unipath_lighter = UniPath::new();
        let cache = Cache::new(settings.cache_threshold);
        let inner = Arc::new(Inner {settings, direct_lighter, unipath_lighter, cache});
        return IrradianceCached {inner};
    }
}

impl Lighter for IrradianceCached {
    fn light(&self, isect: &object::Intersection, sampler: &mut dyn object::Sampler) -> Radiance {
        let surface = &isect.primitive.surface;
        let pnt = isect.point;
        let nrm_facing = isect.facing_normal;
        let albedo = isect.albedo;

        let mut rad = self.inner.direct_lighter.light(isect, sampler);

        if surface.lambert > 0.0 {
            let irad = self.inner.cache.interpolate(pnt, nrm_facing);
            rad += irad * albedo * surface.lambert / PI;
        }

        return rad;
    }

    fn create_prerender_jobs(&self, scene: Arc<Scene>, framebuffer: Arc<Mutex<Framebuffer>>) -> Vec<Box<dyn ExecutorJob>> {
        let width;
        let height;
        if let Ok(framebuffer) = framebuffer.lock() {
            width = framebuffer.width;
            height = framebuffer.height;
        } else {
            return Vec::new();
        }

        let inner = self.inner.clone();
        let job = Box::new(RasterJob::new(
            width,
            height,
            1,
            move |x, y, _sample, thread_local: &mut ThreadLocal| {
                inner.prerender_pixel(x, y, &framebuffer, &scene, &mut thread_local.sampler);
            },
            || {},
            move || {
                let sampler = Random::new();
                return Box::new(ThreadLocal { sampler });
            },
        ));
        
        return vec![job];
    }
}