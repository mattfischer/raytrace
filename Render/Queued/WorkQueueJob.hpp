#ifndef RENDER_QUEUED_WORKQUEUE_JOB_HPP
#define RENDER_QUEUED_WORKQUEUE_JOB_HPP

#include "Render/Executor.hpp"
#include "Render/Queued/WorkQueue.hpp"

#include <functional>

namespace Render {
        namespace Queued {
        class WorkQueueJob : public Executor::Job {
        public:
            typedef std::function<std::unique_ptr<WorkQueue::ThreadLocal>()> CreateThreadLocalFunc;
            WorkQueueJob(WorkQueue &workQueue, CreateThreadLocalFunc createThreadLocalFunc);

            std::unique_ptr<Executor::Job::ThreadLocal> createThreadLocal() override;
            bool execute(Executor::Job::ThreadLocal &threadLocal) override;
        
        private:
            struct ThreadLocal : public Executor::Job::ThreadLocal
            {
                std::unique_ptr<WorkQueue::ThreadLocal> queueThreadLocal;
            };
            WorkQueue &mWorkQueue;        
            CreateThreadLocalFunc mCreateThreadLocalFunc;
        };
    }
}
#endif