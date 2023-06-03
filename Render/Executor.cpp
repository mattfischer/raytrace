#include "Render/Executor.hpp"

#include <windows.h>

namespace Render {
    Executor::Executor()
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

    void Executor::addJob(std::unique_ptr<Job> job)
    {
        mJobs.push_back(std::move(job));
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
        int jobIndex = 0;

        for(std::unique_ptr<Job> &job : mJobs) {
            std::unique_ptr<Job::ThreadLocal> threadLocal = job->createThreadLocal();;

            while(mRunThreads) {
                if(!job->execute(*threadLocal)) {
                    break;
                }
            }

            if(!mRunThreads) {
                break;
            }            
        }

        if(--mNumRunningThreads == 0) {
            auto endTime = std::chrono::steady_clock::now();
            std::chrono::duration<double> duration = endTime - mStartTime;
            mListener->onExecutorDone(duration.count());
        }
    }
}