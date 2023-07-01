#include "Render/Gpu/WorkQueue.hpp"

namespace Render {
    namespace Gpu {
        WorkQueue::WorkQueue(size_t size, OpenCL::Allocator &allocator)
        {
            mData = (Key*)allocator.allocateBytes(sizeof(Key) * (size + 2));
            mData[0] = 0;
            mData[1] = 0;
            mSize = size;
        }

        WorkQueue::Key WorkQueue::getNextKey()
        {
            int read = mData[0]++;
            if(read >= mData[1]) {
                return kInvalidKey;
            }

            return mData[read + 2];
        }

        bool WorkQueue::addItem(Key key)
        {
            int write = mData[1]++;
            if(write >= mSize) {
                return false;
            }
            mData[write + 2] = key;
            return true;
        }

        int WorkQueue::numQueued()
        {
            return mData[1] - mData[0];
        }

        void WorkQueue::resetRead()
        {
            mData[0] = 0;
        }

        void WorkQueue::clear()
        {
            mData[0] = 0;
            mData[1] = 0;
        }
    }
}