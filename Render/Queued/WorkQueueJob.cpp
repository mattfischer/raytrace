#include "Render/Queued/WorkQueueJob.hpp"

namespace Render {
        namespace Queued {
        WorkQueueJob::WorkQueueJob(WorkQueue &workQueue, CreateThreadLocalFunc createThreadLocalFunc)
        : mWorkQueue(workQueue) 
        , mCreateThreadLocalFunc(std::move(createThreadLocalFunc))
        {
        }

        std::unique_ptr<Executor::Job::ThreadLocal> WorkQueueJob::createThreadLocal()
        {
            std::unique_ptr<ThreadLocal> threadLocal = std::make_unique<ThreadLocal>();
            threadLocal->queueThreadLocal = mCreateThreadLocalFunc();

            return threadLocal;
        }

        bool WorkQueueJob::execute(Executor::Job::ThreadLocal &threadLocalBase)
        {
            ThreadLocal &threadLocal = static_cast<ThreadLocal&>(threadLocalBase);
            return mWorkQueue.executeNext(*threadLocal.queueThreadLocal);
        }
    }
}