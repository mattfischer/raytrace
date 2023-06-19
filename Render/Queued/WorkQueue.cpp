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
            while(true) {
                int read = mRead.load(std::memory_order_acquire);
                int newRead = read + 1;
                if(read == mWrite.load(std::memory_order_acquire)) {
                    return kInvalidKey;
                }

                Key key = mQueue[read];    
                if(mRead.compare_exchange_weak(read, newRead, std::memory_order_acq_rel)) {
                    return key;
                }
            }
        }

        bool WorkQueue::addItem(Key key)
        {
            while(true) {
                int write = mWrite.load(std::memory_order_acquire);
                int newWrite = write + 1;
                if(write == mQueue.size()) {
                    return false;
                }

                if(mWrite.compare_exchange_weak(write, newWrite, std::memory_order_acq_rel)) {                
                    mQueue[write] = key;
                    return true;
                }
            }
        }

        int WorkQueue::numQueued()
        {
            if(mWrite >= mRead) {
                return mWrite - mRead;
            } else {
                return mQueue.size() + mWrite - mRead;
            }
        }

        void WorkQueue::clear()
        {
            mWrite = 0;
            mRead = 0;
        }
    }
}