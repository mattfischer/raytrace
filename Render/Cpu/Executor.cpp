#include "Render/Cpu/Executor.hpp"

#include <windows.h>

namespace Render {
    namespace Cpu {
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

        void Executor::runJob(std::unique_ptr<Job> job, JobDoneFunc jobDoneFunc)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            mCurrentJob = std::move(job);
            mJobDoneFunc = std::move(jobDoneFunc);
            mRunJob = true;

            mCondVar.notify_all();
        }

        void Executor::stop()
        {
            mRunJob = false;
            mCondVar.notify_all();
        }

        bool Executor::running()
        {
            return mNumRunningThreads > 0;
        }

        void Executor::runThread()
        {
            while(true) {
                {
                    std::unique_lock<std::mutex> lock(mMutex);
                    while(!mCurrentJob || !mRunJob) {
                        mCondVar.wait(lock);
                        if(!mRunThreads) {
                            break;
                        }
                    }
                    mNumRunningThreads++;
                }
                
                if(!mRunThreads) {
                    break;
                }

                std::unique_ptr<Job::ThreadLocal> threadLocal = mCurrentJob->createThreadLocal();
                bool jobDone = false;
                while(mRunThreads && mRunJob && !jobDone) {
                    if(!mCurrentJob->execute(*threadLocal)) {
                        jobDone = true;
                    }
                }

                std::unique_lock<std::mutex> lock(mMutex);
                if(!mRunThreads) {
                    break;
                }

                if(--mNumRunningThreads == 0) {
                    if(jobDone) {
                        auto jobDoneFunc = std::move(mJobDoneFunc);
                        std::unique_ptr<Job> job = std::move(mCurrentJob);
                        lock.unlock();
                        job->done();
                        if(jobDoneFunc) {
                            jobDoneFunc();
                        }
                    }
                }
            }
        }
    }
}