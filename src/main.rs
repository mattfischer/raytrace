mod geo;
mod input;
mod object;
mod render;

fn main() {    
    let scene = input::SceneParser::parse_scene("foo".to_string());

    println!("{scene:#?}");
}
