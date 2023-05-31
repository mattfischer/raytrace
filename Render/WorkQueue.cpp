#include "Render/WorkQueue.hpp"

#include <thread>

namespace Render {
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
            int read = mRead;
            int newRead = (read + 1) % mQueue.size();
            if(read == mCommitted) {
                return false;
            }

            Key key = mQueue[read];    
            if(mRead.compare_exchange_weak(read, newRead)) {
                mWorkerFunction(key, threadLocal);
                return true;
            }
        }
    }

    bool WorkQueue::addItem(Key key)
    {
        while(true) {
            int write = mWrite;
            int newWrite = (write + 1) % mQueue.size();
            if(newWrite == mRead) {
                std::this_thread::yield();
                continue;
            }

            if(mWrite.compare_exchange_weak(write, newWrite)) {                
                mQueue[write] = key;

                while(true) {
                    int expected = write;
                    if(mCommitted.compare_exchange_weak(expected, newWrite)) {
                        break;
                    } else {
                        std::this_thread::yield();
                    }
                }
                return true;
            }
        }
    }
}