mod geo;
mod object;

fn main() {    
    let shape = object::shape::Sphere::new(geo::Point3::new(1.0, 2.0, 3.0), 5.0);
    let albedo = object::albedo::Solid::new(object::Color::new(1.0, 0.0, 0.0));
    let surface = object::Surface::new(Box::new(albedo));
    let p = object::Primitive::new(Box::new(shape), surface);

    println!("Primitive is {p:#?}");
}
