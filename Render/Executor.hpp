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

        template<typename ThreadLocalType> class FuncJob : public Job {
        public:
            typedef std::function<bool(ThreadLocalType &)> ExecuteFunc;
            FuncJob(ExecuteFunc executeFunc) : mExecuteFunc(std::move(executeFunc)) {}

            std::unique_ptr<Job::ThreadLocal> createThreadLocal() { return std::make_unique<ThreadLocalType>(); }

            bool execute(Job::ThreadLocal &threadLocal) {
                return mExecuteFunc(static_cast<ThreadLocalType&>(threadLocal));
            }

        private:
            ExecuteFunc mExecuteFunc;
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