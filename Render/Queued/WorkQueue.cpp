#include "Render/Queued/WorkQueue.hpp"

namespace Render {
    namespace Queued {
        WorkQueue::WorkQueue(size_t size)
        : mQueue(size)
        {
            mRead = 0;
            mWrite = 0;
        }

        WorkQueue::Key WorkQueue::getNextKey()
        {
            int read = mRead++;
            if(read >= mWrite) {
                return kInvalidKey;
            }

            return mQueue[read];
        }

        bool WorkQueue::addItem(Key key)
        {
            int write = mWrite++;
            if(write >= mQueue.size()) {
                return false;
            }
            mQueue[write] = key;
            return true;
        }

        int WorkQueue::numQueued()
        {
            return mWrite - mRead;
        }

        void WorkQueue::clear()
        {
            mWrite = 0;
            mRead = 0;
        }
    }
}