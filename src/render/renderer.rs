pub mod restir;
pub use restir::ReSTIR;
pub use restir::ReSTIRSettings;

pub mod simple;
pub use simple::Simple;
pub use simple::SimpleSettings;

pub trait Renderer: Send + Sync {
    fn start(&self, done: Box<dyn FnOnce(f32) + 'static + Send + Sync>);
    fn stop(&self);
    fn running(&self) -> bool;
    fn framebuffer_ptr(&self) -> *const u8;
}
