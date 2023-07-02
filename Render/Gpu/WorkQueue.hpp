#ifndef RENDER_GPU_WORKQUEUE_HPP
#define RENDER_GPU_WORKQUEUE_HPP

#include "OpenCL.hpp"
#include "Proxies.hpp"

#include <vector>
#include <functional>
#include <atomic>

namespace Render {
    namespace Gpu {
        class WorkQueue {
        public:
            typedef uint32_t Key;
            static const Key kInvalidKey = UINT_MAX;

            WorkQueue(size_t size, OpenCL::Allocator &allocator);

            Key getNextKey();

            bool addItem(Key key);
            int numQueued();
            void resetRead();
            void clear();

            void writeProxy(WorkQueueProxy &proxy) const;

        private:
            Key *mData;
            size_t mSize;
        };
    }
}

#endif