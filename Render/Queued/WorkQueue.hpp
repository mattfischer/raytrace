#ifndef RENDER_QUEUED_WORKQUEUE_HPP
#define RENDER_QUEUED_WORKQUEUE_HPP

#include <vector>
#include <functional>
#include <atomic>

namespace Render {
    namespace Queued {
        class WorkQueue {
        public:
            typedef uint32_t Key;
            struct ThreadLocal {};
            typedef std::function<void(Key, ThreadLocal&)> WorkerFunction;

            WorkQueue(size_t size, WorkerFunction workerFunction);

            bool executeNext(ThreadLocal &threadLocal);
            bool addItem(Key key);
            int numQueued();

        private:
            WorkerFunction mWorkerFunction;
            std::vector<Key> mQueue;
            std::atomic_int mRead;
            std::atomic_int mWrite;
            std::atomic_int mCommitted;
        };
    }
}

#endif