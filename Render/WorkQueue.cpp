#include "Render/WorkQueue.hpp"

namespace Render {
    WorkQueue::WorkQueue(size_t size, WorkerFunction workerFunction)
    : mQueue(size), mWorkerFunction(std::move(workerFunction))
    {
        mHead = -1;
        mTail = 0;
    }

    bool WorkQueue::executeNext(ThreadLocal &threadLocal)
    {
        std::unique_lock<std::mutex> lock(mMutex);

        if(mHead == -1) {
            return false;
        } else {
            Key key = mQueue[mHead];
            if(mTail == -1) {
                mTail = mHead;
            }
            mHead = (mHead + 1) % mQueue.size();
            if(mHead == mTail) {
                mHead = -1;
            }
            
            lock.unlock();

            mWorkerFunction(key, threadLocal);
            return true;
        }
    }

    bool WorkQueue::addItem(Key key)
    {
        std::unique_lock<std::mutex> lock(mMutex);

        if(mTail == -1) {
            return false;
        } else {
            mQueue[mTail] = key;
            if(mHead == -1) {
                mHead = mTail;
            }
            mTail = (mTail + 1) % mQueue.size();
            if(mTail == mHead) {
                mTail = -1;
            }
            return true;
        }
    }
}