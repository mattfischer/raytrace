use pyo3::prelude::*;
use pyo3_ffi::c_str;

mod app;
use app::py_interface::py_module;

mod geo;
mod input;
mod object;
mod render;

fn main() {
    pyo3::append_to_inittab!(py_module);

    Python::with_gil(|py| {
        let app_src = c_str!(include_str!("app/app.py"));
        if let Ok(app) = PyModule::from_code(py, app_src, c_str!("app.py"), c_str!("__main__")) {
            let main_window_ui = include_str!("app/MainWindow.ui");
            let _ = app.getattr("run_app").unwrap().call1((main_window_ui,));
        } else {
            println!("Error");
        }
    });
}
