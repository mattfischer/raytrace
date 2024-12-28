use pyo3::exceptions::PyValueError;
use pyo3::prelude::*;

use crate::geo;
use crate::input;
use crate::object;
use crate::render;

use geo::Point2;

use input::SceneParser;

use render::LightProbe;
use render::Renderer;
use render::renderer::Simple;
use render::renderer::SimpleSettings;
use render::renderer::ReSTIR;
use render::renderer::ReSTIRSettings;

use std::ffi::c_void;
use std::sync::Arc;

#[pyclass]
struct Settings {
    #[pyo3(get, set)]
    pub width: usize,

    #[pyo3(get, set)]
    pub height: usize,

    #[pyo3(get, set)]
    pub samples: usize,

    #[pyo3(get, set)]
    pub irradiance_cache_samples: usize,

    #[pyo3(get, set)]
    pub irradiance_cache_threshold: f32,

    #[pyo3(get, set)]
    pub restir_indirect_samples: usize,

    #[pyo3(get, set)]
    pub restir_radius: usize,

    #[pyo3(get, set)]
    pub restir_candidates: usize,

    #[pyo3(get, set)]
    pub render_method: String,
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
            render_method: String::new(),
        };
    }
}

#[pyclass]
struct Scene {
    scene: Option<Arc<object::Scene>>,
}

#[pymethods]
impl Scene {
    #[new]
    pub fn new(filename: String) -> Scene {
        if let Some(scene) = SceneParser::parse_scene(filename) {
            return Scene { scene: Some(Arc::new(scene)) };
        } else {
            return Scene { scene: None };
        }
    }
}

#[pyclass]
struct Framebuffer {
    #[pyo3(get, set)]
    width: usize,

    #[pyo3(get, set)]
    height: usize,

    bits: *const u8,
}

unsafe impl Send for Framebuffer {}
unsafe impl Sync for Framebuffer {}

#[pymethods]
impl Framebuffer {
    pub unsafe fn __getbuffer__(
        slf: PyRefMut<'_, Self>,
        buffer: *mut pyo3_ffi::Py_buffer,
        flags: i32,
    ) {
        let size = (slf.width * slf.height * 3) as isize;
        pyo3_ffi::PyBuffer_FillInfo(
            buffer,
            slf.as_ptr(),
            slf.bits as *mut c_void,
            size,
            1,
            flags,
        );
    }
}

#[pyclass]
struct Engine {
    renderer: Box<dyn Renderer>,
    scene: Arc<object::Scene>,

    #[pyo3(get)]
    pub render_framebuffer: Py<Framebuffer>,
}

#[pymethods]
impl Engine {
    #[new]
    pub fn new(
        py: Python<'_>,
        scene: &Bound<'_, Scene>,
        settings: &Bound<'_, Settings>,
    ) -> PyResult<Engine> {
        if let Some(scene) = &scene.borrow().scene {
            let scene = scene.clone();
            let settings = settings.borrow();
            
            let renderer: Box<dyn Renderer> = 
            match settings.render_method.as_str() {
                "restir" => {
                    let render_settings = ReSTIRSettings {
                        width: settings.width,
                        height: settings.height,
                        samples: settings.samples,
                        indirect_samples: settings.restir_indirect_samples,
                        radius: settings.restir_radius,
                        candidates: settings.restir_candidates
                    };
                    Box::new(ReSTIR::new(scene.clone(), render_settings))
                },
                _ => {
                    let render_settings = SimpleSettings {
                        width: settings.width,
                        height: settings.height,
                        samples: settings.samples,
                    };
                    let lighter: Option<Box<dyn render::Lighter>> = match settings.render_method.as_str() {
                        "directLighting" => Some(Box::new(render::lighter::Direct::new())),
                        "pathTracing" => Some(Box::new(render::lighter::UniPath::new())),
                        _ => None,
                    };

                    Box::new(Simple::new(scene.clone(), render_settings, lighter))
                }
            };
            let render_framebuffer = Py::new(
                py,
                Framebuffer {
                    width: settings.width,
                    height: settings.height,
                    bits: renderer.framebuffer_ptr(),
                },
            )
            .unwrap();
            return Ok(Engine {
                renderer,
                scene,
                render_framebuffer,
            });
        } else {
            return Err(PyValueError::new_err("No scene"));
        }
    }

    pub fn start_render(&mut self, listener: PyObject) {
        let done = move |time| {
            Python::with_gil(|py| {
                if let Ok(func) = listener.getattr(py, "on_render_done") {
                    let _ = func.call1(py, (time,));
                }
            });
        };

        self.renderer.start(Box::new(done));
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

        let beam = self.scene.camera.create_pixel_beam(
            Point2::new(x as f32, y as f32),
            width,
            height,
            Point2::ZERO,
        );
        let mut result = Vec::new();
        if let Some(isect) = self.scene.intersect(beam, f32::MAX, true) {
            let mut probe = LightProbe::new(&isect);

            for _ in 0..1000 {
                let (azimuth, elevation, color) = probe.get_sample();

                result.push(((color.red, color.green, color.blue), azimuth, elevation));
            }
        }
        return result;
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
