#ifndef RENDER_EXECUTOR_HPP
#define RENDER_EXECUTOR_HPP

#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <functional>

namespace Render {
    class Executor {
    public:
        class Job {
        public:
            struct ThreadLocal {};

            virtual ~Job() = default;
            virtual bool execute(ThreadLocal &threadLocal) = 0;
            virtual std::unique_ptr<ThreadLocal> createThreadLocal() { return nullptr; }
        };

        Executor();
        ~Executor();

        typedef std::function<void()> JobDoneFunc;
        void runJob(Job &job, JobDoneFunc jobDoneFunc);
        void stop();
        bool running();

    private:
        void runThread();

        std::vector<std::unique_ptr<std::thread>> mThreads;

        std::atomic_bool mRunThreads;
        std::atomic_bool mRunJob;

        std::mutex mMutex;
        std::condition_variable mCondVar;
        Job *mCurrentJob;
        JobDoneFunc mJobDoneFunc;
        int mNumRunningThreads;
    };
}
#endif