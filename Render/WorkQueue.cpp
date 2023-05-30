#include "Render/WorkQueue.hpp"

#include <windows.h>

namespace Render {
    struct WorkQueue::priv {
        CRITICAL_SECTION criticalSection;
    };

    WorkQueue::WorkQueue(size_t size, WorkerFunction workerFunction)
    : mQueue(size), mWorkerFunction(std::move(workerFunction))
    {
        mHead = -1;
        mTail = 0;
        mPriv = new priv;
        InitializeCriticalSection(&mPriv->criticalSection);
    }

    bool WorkQueue::executeNext(ThreadLocal &threadLocal)
    {
        EnterCriticalSection(&mPriv->criticalSection);

        if(mHead == -1) {
            LeaveCriticalSection(&mPriv->criticalSection);
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
            
            LeaveCriticalSection(&mPriv->criticalSection);

            mWorkerFunction(key, threadLocal);
            return true;
        }
    }

    bool WorkQueue::addItem(Key key)
    {
        EnterCriticalSection(&mPriv->criticalSection);

        if(mTail == -1) {
            LeaveCriticalSection(&mPriv->criticalSection);
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
            LeaveCriticalSection(&mPriv->criticalSection);
            return true;
        }
    }
}