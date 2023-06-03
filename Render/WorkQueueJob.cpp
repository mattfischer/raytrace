#include "Render/WorkQueueJob.hpp"

namespace Render {
    WorkQueueJob::WorkQueueJob(CreateThreadLocalFunc createThreadLocalFunc)
    : mCreateThreadLocalFunc(std::move(createThreadLocalFunc))
    {
    }

    void WorkQueueJob::addWorkQueue(WorkQueue &workQueue)
    {
        mWorkQueues.push_back(workQueue);
    }

    std::unique_ptr<Executor::Job::ThreadLocal> WorkQueueJob::createThreadLocal()
    {
        std::unique_ptr<ThreadLocal> threadLocal = std::make_unique<ThreadLocal>();
        threadLocal->currentQueue = 0;
        threadLocal->queueThreadLocal = mCreateThreadLocalFunc();

        return threadLocal;
    }

    bool WorkQueueJob::execute(Executor::Job::ThreadLocal &threadLocalBase)
    {
        ThreadLocal &threadLocal = static_cast<ThreadLocal&>(threadLocalBase);
        int startQueue = threadLocal.currentQueue;
        while(!mWorkQueues[threadLocal.currentQueue].get().executeNext(*threadLocal.queueThreadLocal)) {
            threadLocal.currentQueue = (threadLocal.currentQueue + 1) % mWorkQueues.size();
            if(threadLocal.currentQueue == startQueue) {
                return false;
            }
        }

        return true;
    }
}