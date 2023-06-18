#include "Render/Queued/WorkQueue.hpp"

namespace Render {
    namespace Queued {
        WorkQueue::WorkQueue(size_t size, WorkerFunction workerFunction)
        : mQueue(size), mWorkerFunction(std::move(workerFunction))
        {
            mRead = 0;
            mWrite = 0;
            mCommitted = 0;
        }

        bool WorkQueue::executeNext(ThreadLocal &threadLocal)
        {
            while(true) {
                int read = mRead.load(std::memory_order_acquire);
                int newRead = (read + 1) % mQueue.size();
                if(read == mCommitted.load(std::memory_order_acquire)) {
                    return false;
                }

                Key key = mQueue[read];    
                if(mRead.compare_exchange_weak(read, newRead, std::memory_order_acq_rel)) {
                    mWorkerFunction(key, threadLocal);
                    return true;
                }
            }
        }

        bool WorkQueue::addItem(Key key)
        {
            while(true) {
                int write = mWrite.load(std::memory_order_acquire);
                int newWrite = (write + 1) % mQueue.size();
                if(newWrite == mRead.load(std::memory_order_acquire)) {
                    continue;
                }

                if(mWrite.compare_exchange_weak(write, newWrite, std::memory_order_acq_rel)) {                
                    mQueue[write] = key;

                    while(true) {
                        int expected = write;
                        if(mCommitted.compare_exchange_weak(expected, newWrite, std::memory_order_acq_rel)) {
                            break;
                        }
                    }
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
    }
}