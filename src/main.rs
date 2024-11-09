mod geo;
mod object;

fn main() {    
    let shape = object::shape::Sphere::new(geo::Point3::new(1.0, 2.0, 3.0), 5.0);
    let albedo = object::albedo::Solid::new(object::Color::new(1.0, 0.0, 0.0));
    let brdf = object::brdf::Lambert::new(1.0);
    let surface = object::Surface::new(Box::new(albedo), vec![Box::new(brdf)]);
    let primitive = object::Primitive::new(Box::new(shape), surface);
    let camera = object::Camera::new(geo::Point3::new(0.0, 0.0, 0.0), geo::Vec3::new(0.0, 0.0, 0.0), geo::Vec3::new(0.0, 0.0, 0.0), 0.0, 0.0, 0.0);
    let scene = object::Scene::new(camera, vec![primitive]);

    println!("{scene:#?}");
}
