#ifndef RENDER_CPU_EXECUTOR_HPP
#define RENDER_CPU_EXECUTOR_HPP

#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <functional>

namespace Render {
    namespace Cpu {
        class Executor {
        public:
            class Job {
            public:
                struct ThreadLocal {};

                virtual ~Job() = default;
                virtual bool execute(ThreadLocal &threadLocal) = 0;
                virtual std::unique_ptr<ThreadLocal> createThreadLocal() = 0;
                virtual void done() = 0;
            };

            template<typename ThreadLocalType> class FuncJob : public Job {
            public:
                typedef std::function<bool(ThreadLocalType &)> ExecuteFunc;
                typedef std::function<void()> DoneFunc;
                FuncJob(ExecuteFunc executeFunc, DoneFunc doneFunc) 
                : mExecuteFunc(std::move(executeFunc))
                , mDoneFunc(std::move(doneFunc))
                {
                }

                std::unique_ptr<Job::ThreadLocal> createThreadLocal() override
                {
                    return std::make_unique<ThreadLocalType>();
                }

                bool execute(Job::ThreadLocal &threadLocal) override
                {
                    return mExecuteFunc(static_cast<ThreadLocalType&>(threadLocal));
                }

                void done() override
                {
                    mDoneFunc();
                }

            private:
                ExecuteFunc mExecuteFunc;
                DoneFunc mDoneFunc;
            };

            Executor();
            ~Executor();

            typedef std::function<void()> JobDoneFunc;
            void runJob(std::unique_ptr<Job> job, JobDoneFunc jobDoneFunc = JobDoneFunc());
            void stop();
            bool running();

        private:
            void runThread();

            std::vector<std::unique_ptr<std::thread>> mThreads;

            std::atomic_bool mRunThreads;
            std::atomic_bool mRunJob;

            std::mutex mMutex;
            std::condition_variable mCondVar;
            std::unique_ptr<Job> mCurrentJob;
            JobDoneFunc mJobDoneFunc;
            int mNumRunningThreads;
        };
    }
}
#endif