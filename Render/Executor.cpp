#include "Render/Executor.hpp"

#include <windows.h>

namespace Render {
    Executor::Executor(ThreadLocalCreator threadLocalCreator)
    : mThreadLocalCreator(std::move(threadLocalCreator))
    {
        mRunThreads = false;
        mNumRunningThreads = 0;
        mListener = nullptr;
    }

    Executor::~Executor()
    {
        mRunThreads = false;
        for(std::unique_ptr<std::thread> &thread : mThreads) {
            thread->join();
        }
        mThreads.clear();
    }

    void Executor::addWorkQueue(WorkQueue &workQueue)
    {
        mWorkQueues.push_back(workQueue);
    }

    void Executor::start(Listener *listener)
    {
        if(mRunThreads) {
            return;
        }

        for(std::unique_ptr<std::thread> &thread : mThreads) {
            thread->join();
        }
        mThreads.clear();
        mListener = listener;
        mRunThreads = true;
        mStartTime = std::chrono::steady_clock::now();

        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        unsigned int numThreads = static_cast<unsigned int>(sysinfo.dwNumberOfProcessors);

        for(int i=0; i<numThreads; i++) {
            mThreads.push_back(std::make_unique<std::thread>([&]() { runThread(); }));
            mNumRunningThreads++;
        }
    }

    void Executor::stop()
    {
        mRunThreads = false;
    }

    bool Executor::running()
    {
        return mNumRunningThreads > 0;
    }

    void Executor::runThread()
    {
        bool progress = false;
        int queueIndex = 0;
        std::unique_ptr<WorkQueue::ThreadLocal> threadLocal = mThreadLocalCreator();

        while(true) {
            if(!mRunThreads) {
                break;
            }

            WorkQueue &workQueue = mWorkQueues[queueIndex];
            while(workQueue.executeNext(*threadLocal)) {
                progress = true;
            }

            queueIndex = (queueIndex + 1) % mWorkQueues.size();
            if(queueIndex == 0) {
                if(!progress) {
                    break;
                }
                progress = false;
            }
        }

        if(--mNumRunningThreads == 0) {
            auto endTime = std::chrono::steady_clock::now();
            std::chrono::duration<double> duration = endTime - mStartTime;
            mListener->onExecutorDone((int)duration.count());
        }
    }
}