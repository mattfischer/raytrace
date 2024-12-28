use std::any::Any;
use std::ops::DerefMut;
use std::sync::atomic::AtomicBool;
use std::sync::atomic::AtomicUsize;
use std::sync::atomic::Ordering;
use std::sync::mpsc::Receiver;
use std::sync::mpsc::Sender;
use std::sync::Arc;

pub trait ExecutorJob: Send + Sync {
    fn execute(&self, thread_local: &mut dyn Any) -> bool;
    fn create_thread_local(&self) -> Box<dyn Any>;
    fn done(&self);
}

enum Command {
    RunJob(
        Arc<Box<dyn ExecutorJob>>,
        Arc<Box<dyn FnOnce() + Send + Sync>>,
    ),
    Exit,
}

struct ThreadInfo {
    handle: std::thread::JoinHandle<()>,
    sender: Sender<Command>,
}

struct SharedState {
    num_running: AtomicUsize,
    run_jobs: AtomicBool,
}
pub struct Executor {
    threads: Vec<ThreadInfo>,
    shared_state: Arc<SharedState>,
}

impl Executor {
    fn run_thread(receiver: Receiver<Command>, shared_state: Arc<SharedState>) {
        loop {
            if let Ok(command) = receiver.recv() {
                match command {
                    Command::RunJob(job, done) => {
                        shared_state.num_running.fetch_add(1, Ordering::SeqCst);
                        let mut job_done = false;
                        let mut thread_local = job.create_thread_local();
                        while !job_done && shared_state.run_jobs.load(Ordering::SeqCst) {
                            if !job.execute(thread_local.deref_mut()) {
                                job_done = true;
                            }
                        }
                        shared_state.num_running.fetch_sub(1, Ordering::SeqCst);
                        if job_done {
                            if let Some(done) = Arc::into_inner(done) {
                                job.done();
                                done();
                            }
                        }
                    }

                    Command::Exit => break,
                }
            }
        }
    }

    pub fn new() -> Executor {
        let mut threads = Vec::new();
        let shared_state = Arc::new(SharedState {
            num_running: AtomicUsize::new(0),
            run_jobs: AtomicBool::new(true),
        });

        let num_threads = match std::thread::available_parallelism() {
            Ok(num) => num.into(),
            Err(_) => 1,
        };

        for _ in 0..num_threads {
            let (sender, receiver) = std::sync::mpsc::channel();
            let shared_state = shared_state.clone();
            let handle = std::thread::spawn(move || {
                Self::run_thread(receiver, shared_state);
            });
            threads.push(ThreadInfo { handle, sender });
        }

        return Executor {
            threads,
            shared_state,
        };
    }

    pub fn run_job<D>(&self, job: Box<dyn ExecutorJob>, done: D)
    where
        D: FnOnce() + Send + Sync + 'static,
    {
        self.shared_state.run_jobs.store(true, Ordering::SeqCst);
        let j = Arc::new(job);
        let d = Arc::new(Box::new(done) as Box<dyn FnOnce() + Send + Sync>);
        for thread in self.threads.iter() {
            let _ = thread.sender.send(Command::RunJob(j.clone(), d.clone()));
        }
    }

    pub fn stop(&self) {
        self.shared_state.run_jobs.store(false, Ordering::SeqCst);
    }

    pub fn running(&self) -> bool {
        return self.shared_state.num_running.load(Ordering::SeqCst) > 0;
    }
}

impl Drop for Executor {
    fn drop(&mut self) {
        self.shared_state.run_jobs.store(false, Ordering::SeqCst);
        for thread in self.threads.drain(..) {
            let _ = thread.sender.send(Command::Exit);
            let _ = thread.handle.join();
        }
    }
}
