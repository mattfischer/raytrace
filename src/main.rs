mod geo;
mod object;

fn main() {
    let u = geo::Vec3::new(2.0, 3.0, 4.0);
    let v = geo::Vec3::new(1.0, 2.0, 3.0);
    
    let s = object::shape::Sphere::new(geo::Point3::new(1.0, 2.0, 3.0), 5.0);
    let z = u + v;
    println!("Vector is {z}");
}
