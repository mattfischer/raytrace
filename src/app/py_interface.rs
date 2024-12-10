use pyo3::prelude::*;
use pyo3::exceptions::PyValueError;

use crate::geo;
use crate::input;
use crate::object;
use crate::render;

use geo::Point2;

use input::SceneParser;

use render::LightProbe;
use render::Renderer;
use render::RendererSettings;

use std::ffi::c_void;

#[pyclass]
struct Settings {
    #[pyo3(get, set)]
    pub width : usize,

    #[pyo3(get, set)]
    pub height : usize,

    #[pyo3(get, set)]
    pub samples : usize,

    #[pyo3(get, set)]
    pub irradiance_cache_samples : usize,

    #[pyo3(get, set)]
    pub irradiance_cache_threshold : f32,

    #[pyo3(get, set)]
    pub restir_indirect_samples : usize,

    #[pyo3(get, set)]
    pub restir_radius : usize,

    #[pyo3(get, set)]
    pub restir_candidates : usize,

    #[pyo3(get, set)]
    pub render_method : String
}

#[pymethods]
impl Settings {
    #[new]
    pub fn new() -> Settings {
        return Settings {
            width: 0,
            height: 0,
            samples: 0,
            irradiance_cache_samples: 0,
            irradiance_cache_threshold: 0.0,
            restir_indirect_samples: 0,
            restir_radius: 0,
            restir_candidates: 0,
            render_method: String::new()};
    }
}

#[pyclass]
struct Scene {
    scene : Option<object::Scene>
}

#[pymethods]
impl Scene {
    #[new]
    pub fn new(filename : String) -> Scene {
        let scene = SceneParser::parse_scene(filename);
        return Scene {scene};
    }
}

#[pyclass]
struct Framebuffer {
    #[pyo3(get, set)]
    width : usize,

    #[pyo3(get, set)]
    height : usize,

    bits : usize
}

#[pymethods]
impl Framebuffer {
    pub unsafe fn __getbuffer__(slf: PyRefMut<'_, Self>, buffer : *mut pyo3_ffi::Py_buffer, flags : i32) {
        let size = (slf.width * slf.height * 3) as isize;
        pyo3_ffi::PyBuffer_FillInfo(buffer, slf.as_ptr(), slf.bits as *mut c_void, size, 1, flags);
    }
}

#[pyclass]
struct Engine {
    renderer : Renderer,

    #[pyo3(get)]
    pub render_framebuffer : Py<Framebuffer>
}

#[pymethods]
impl Engine {
    #[new]
    pub fn new(py : Python<'_>, scene : &Bound<'_, Scene>, settings : &Bound<'_, Settings>) -> PyResult<Engine> {
        if let Some(scene) = scene.borrow_mut().scene.take() {
            let settings = settings.borrow();
            let render_settings = RendererSettings {width: settings.width, height: settings.height, samples: settings.samples};
            let lighter : Option<Box<dyn render::Lighter>> = match settings.render_method.as_str() {
                "directLighting" => Some(Box::new(render::lighter::Direct::new())),
                "pathTracing" => Some(Box::new(render::lighter::UniPath::new())),
                _ => None
            };

            let renderer = Renderer::new(scene, render_settings, lighter);
            let render_framebuffer = Py::new(py, Framebuffer{width: settings.width, height: settings.height, bits: renderer.framebuffer_ptr() as usize}).unwrap();
            return Ok(Engine {renderer, render_framebuffer });
        } else {
            return Err(PyValueError::new_err("No scene"));
        }
    }

    pub fn start_render(&mut self, listener: PyObject) {
        self.renderer.start(move |time| {
            Python::with_gil(|py| {
                if let Ok(func) = listener.getattr(py, "on_render_done") {
                    let _ = func.call1(py, (time,));
                }
            });
        });
    }

    pub fn stop_render(&self) {
        self.renderer.stop();
    }

    pub fn rendering(&self) -> bool {
        return self.renderer.running();
    }

    pub fn render_probe(&self, py: Python, x: usize, y: usize) -> Vec<((f32, f32, f32), f32, f32)> {
        let width = self.render_framebuffer.borrow(py).width;
        let height = self.render_framebuffer.borrow(py).height;

        return self.renderer.run_with_scene(|scene| {
            let beam = scene.camera.create_pixel_beam(Point2::new(x as f32, y as f32), width, height, Point2::ZERO);
            let mut result = Vec::new();
            if let Some(isect) = scene.intersect(beam, f32::MAX, true) {
                let mut probe = LightProbe::new(&isect);
    
                for _ in 0..1000 {
                    let (azimuth, elevation, color) = probe.get_sample();
        
                    result.push(((color.red, color.green, color.blue), azimuth, elevation));
                }
            }
            return result;
        });
    }
}

#[pymodule(name = "raytrace")]
pub fn py_module(m: &Bound<'_, PyModule>) -> PyResult<()> {
    m.add_class::<Settings>()?;
    m.add_class::<Scene>()?;
    m.add_class::<Engine>()?;
    m.add_class::<Framebuffer>()?;

    Ok(())
}