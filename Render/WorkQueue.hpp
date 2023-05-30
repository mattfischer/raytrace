#ifndef RENDER_WORKQUEUE_HPP
#define RENDER_WORKQUEUE_HPP

#include <vector>
#include <functional>
#include <mutex>

namespace Render {
    class WorkQueue {
    public:
        typedef uint32_t Key;
        struct ThreadLocal {};
        typedef std::function<void(Key, ThreadLocal&)> WorkerFunction;

        WorkQueue(size_t size, WorkerFunction workerFunction);

        bool executeNext(ThreadLocal &threadLocal);
        bool addItem(Key key);

    private:
        WorkerFunction mWorkerFunction;
        std::vector<Key> mQueue;
        std::mutex mMutex;
        size_t mHead;
        size_t mTail;
    };
}

#endif