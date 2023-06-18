#include "Render/Executor.hpp"

#include <windows.h>

namespace Render {
    Executor::Executor()
    {
        mNumRunningThreads = 0;
        mCurrentJob = nullptr;
        mRunThreads = true;

        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        unsigned int numThreads = static_cast<unsigned int>(sysinfo.dwNumberOfProcessors);

        for(int i=0; i<numThreads; i++) {
            mThreads.push_back(std::make_unique<std::thread>([&]() { runThread(); }));
        }
    }

    Executor::~Executor()
    {
        mRunThreads = false;
        mCondVar.notify_all();
        for(std::unique_ptr<std::thread> &thread : mThreads) {
            thread->join();
        }
        mThreads.clear();
    }

    void Executor::runJob(Job &job, JobDoneFunc jobDoneFunc)
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mCurrentJob = &job;
        mJobDoneFunc = jobDoneFunc;
        mRunJob = true;

        mCondVar.notify_all();
    }

    void Executor::stop()
    {
        mRunJob = false;
    }

    bool Executor::running()
    {
        return mNumRunningThreads > 0;
    }

    void Executor::runThread()
    {
        while(true) {
            Job *job = nullptr;
            {
                std::unique_lock<std::mutex> lock(mMutex);
                while(!mCurrentJob) {
                    mCondVar.wait(lock);
                    if(!mRunThreads) {
                        break;
                    }
                }
                job = mCurrentJob;
            }
            
            if(!mRunThreads) {
                break;
            }

            std::unique_ptr<Job::ThreadLocal> threadLocal = job->createThreadLocal();
            mNumRunningThreads++;
            bool jobDone = false;
            while(mRunThreads && mRunJob && !jobDone) {
                if(!job->execute(*threadLocal)) {
                    jobDone = true;
                }
            }

            if(--mNumRunningThreads == 0) {
                mCurrentJob = nullptr;
                if(jobDone) {
                    mJobDoneFunc();
                }
            }
        }
    }
}