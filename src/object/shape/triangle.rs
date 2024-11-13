use crate::geo;
use crate::object;

use geo::Ray;
use geo::Point3;
use geo::Point2;

pub struct Triangle;

impl Triangle {
    pub fn intersect(ray : Ray, p : Point3, pu : Point3, pv : Point3, distance : &mut f32) -> Option<(f32, f32)> {
        let e1 = pu - p;
        let e2 = pv - p;
        let pp = ray.direction % e2;

        let den = pp * e1;
        if den > -1.0e-10 && den < 1.0e-10 {
            return None;
        }

        let iden = 1.0 / den;
        let t = ray.origin - p;
        let uu = (pp * t) * iden;
        if uu < 0.0 || uu > 1.0 {
            return None;
        }

        let q = t % e1;
        let vv = (q * ray.direction) * iden;
        if vv < 0.0 || uu + vv > 1.0 {
            return None;
        }

        let d = (q * e2) * iden;
        if d < 0.0 || d >= *distance {
            return None;
        }

        *distance = d;
        return Some((uu, vv));
    }
}