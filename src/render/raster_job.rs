use crate::render;

use render::ExecutorJob;

use std::sync::atomic::AtomicU64;
use std::sync::atomic::Ordering;

use std::any::Any;

pub struct RasterJob<E, D, C> {
    pub width: usize,
    pub height: usize,
    pub iterations: usize,
    pixel_index: AtomicU64,
    execute_func: E,
    done_func: D,
    create_thread_local_func: C,
}

impl<E, D, C, T> RasterJob<E, D, C>
where
    E: Fn(usize, usize, usize, &mut T) + Sync + Send,
    D: Fn() + Sync + Send,
    C: Fn() -> Box<T> + Sync + Send,
    T: 'static,
{
    pub fn new(
        width: usize,
        height: usize,
        iterations: usize,
        execute_func: E,
        done_func: D,
        create_thread_local_func: C,
    ) -> RasterJob<E, D, C> {
        let pixel_index = AtomicU64::new(0);
        return RasterJob {
            width,
            height,
            iterations,
            pixel_index,
            execute_func,
            done_func,
            create_thread_local_func,
        };
    }
}

impl<E, D, C, T> ExecutorJob for RasterJob<E, D, C>
where
    E: Fn(usize, usize, usize, &mut T) + Sync + Send,
    D: Fn() + Sync + Send,
    C: Fn() -> Box<T> + Sync + Send,
    T: 'static,
{
    fn execute(&self, thread_local: &mut dyn Any) -> bool {
        let pixel_index = self.pixel_index.fetch_add(1, Ordering::SeqCst);
        let iteration = (pixel_index / ((self.width * self.height) as u64)) as usize;
        let y = ((pixel_index / (self.width as u64)) % (self.height as u64)) as usize;
        let x = (pixel_index % (self.width as u64)) as usize;

        if iteration >= self.iterations {
            return false;
        }

        if let Some(thread_local) = thread_local.downcast_mut() {
            (self.execute_func)(x, y, iteration, thread_local);
        }

        return true;
    }

    fn done(&self) {
        (self.done_func)();
    }

    fn create_thread_local(&self) -> Box<dyn Any> {
        return (self.create_thread_local_func)();
    }
}
