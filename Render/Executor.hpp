#ifndef RENDER_EXECUTOR_HPP
#define RENDER_EXECUTOR_HPP

#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>

namespace Render {
    class Executor {
    public:
        class Listener {
        public:
            virtual ~Listener() = default;
            virtual void onExecutorDone(float totalTimeSeconds) = 0;
        };

        class Job {
        public:
            struct ThreadLocal {};

            virtual ~Job() = default;
            virtual bool execute(ThreadLocal &threadLocal) = 0;
            virtual std::unique_ptr<ThreadLocal> createThreadLocal() { return nullptr; }
        };

        Executor();
        ~Executor();

        void addJob(std::unique_ptr<Job> job);

        void start(Listener *listener);
        void stop();
        bool running();

    private:
        void runThread();

        std::vector<std::unique_ptr<Job>> mJobs;
        std::vector<std::unique_ptr<std::thread>> mThreads;

        std::atomic_bool mRunThreads;
        std::atomic_int mNumRunningThreads;
        Listener *mListener;
        std::chrono::time_point<std::chrono::steady_clock> mStartTime;
    };
}
#endif