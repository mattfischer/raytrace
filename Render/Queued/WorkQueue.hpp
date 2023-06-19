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
            static const Key kInvalidKey = UINT_MAX;

            WorkQueue(size_t size);

            Key getNextKey();

            bool addItem(Key key);
            int numQueued();

        private:
            std::vector<Key> mQueue;
            std::atomic_int mRead;
            std::atomic_int mWrite;
            std::atomic_int mCommitted;
        };
    }
}

#endif