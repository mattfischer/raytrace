#ifndef RENDER_EXECUTOR_HPP
#define RENDER_EXECUTOR_HPP

#include "Render/WorkQueue.hpp"

#include <memory>
#include <vector>
#include <thread>
#include <chrono>

namespace Render {
    class Executor {
    public:
        class Listener {
        public:
            virtual ~Listener() {}
            virtual void onExecutorDone(int totalTimeSeconds) = 0;
        };

        typedef std::function<std::unique_ptr<WorkQueue::ThreadLocal>()> ThreadLocalCreator;

        Executor(ThreadLocalCreator threadLocalCreator);
        ~Executor();

        void addWorkQueue(WorkQueue &workQueue);

        void start(Listener *listener);
        void stop();
        bool running();

    private:
        void runThread();

        std::vector<std::reference_wrapper<WorkQueue>> mWorkQueues;
        std::vector<std::unique_ptr<std::thread>> mThreads;

        std::atomic_bool mRunThreads;
        std::atomic_int mNumRunningThreads;
        Listener *mListener;
        ThreadLocalCreator mThreadLocalCreator;
        std::chrono::time_point<std::chrono::steady_clock> mStartTime;
    };
}
#endif