#include "Render/RasterJob.hpp"

namespace Render {
    RasterJob::RasterJob(int width, int height, int iterations, CreateThreadLocalFunc createThreadLocalFunc, ExecuteFunc executeFunc)
    : mWidth(width)
    , mHeight(height)
    , mIterations(iterations)
    , mCreateThreadLocalFunc(std::move(createThreadLocalFunc))
    , mExecuteFunc(std::move(executeFunc))
    {
        mPixelIndex = 0;
    }

    std::unique_ptr<Executor::Job::ThreadLocal> RasterJob::createThreadLocal()
    {
        return mCreateThreadLocalFunc();
    }

    bool RasterJob::execute(Executor::Job::ThreadLocal &threadLocal)
    {
        uint64_t pixelIndex = mPixelIndex++;
        int iteration = pixelIndex / (mWidth * mHeight);
        int y = (pixelIndex / mWidth) % mHeight;
        int x = pixelIndex % mWidth;

        if(iteration >= mIterations) {
            return false;
        }

        mExecuteFunc(x, y, iteration, threadLocal);
        return true;
    }
}