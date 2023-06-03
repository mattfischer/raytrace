#ifndef RENDER_WORKQUEUE_JOB_HPP
#define RENDER_WORKQUEUE_JOB_HPP

#include "Render/Executor.hpp"
#include "Render/WorkQueue.hpp"

#include <functional>

namespace Render {
    class WorkQueueJob : public Executor::Job {
    public:
        typedef std::function<std::unique_ptr<WorkQueue::ThreadLocal>()> CreateThreadLocalFunc;
        WorkQueueJob(CreateThreadLocalFunc createThreadLocalFunc);

        void addWorkQueue(WorkQueue &workQueue);

        std::unique_ptr<Executor::Job::ThreadLocal> createThreadLocal() override;
        bool execute(Executor::Job::ThreadLocal &threadLocal) override;
    
    private:
        struct ThreadLocal : public Executor::Job::ThreadLocal
        {
            int currentQueue;
            std::unique_ptr<WorkQueue::ThreadLocal> queueThreadLocal;
        };
        std::vector<std::reference_wrapper<WorkQueue>> mWorkQueues;        
        CreateThreadLocalFunc mCreateThreadLocalFunc;
    };
}
#endif