#ifndef RENDER_CPU_RASTERJOB_HPP
#define RENDER_CPU_RASTERJOB_HPP

#include "Render/Cpu/Executor.hpp"

#include <functional>
#include <atomic>
#include <memory>

namespace Render {
    namespace Cpu {
        class RasterJob : public Executor::Job {
        public:
            typedef std::function<void(int, int, int, Executor::Job::ThreadLocal&)> ExecuteFunc;
            typedef std::function<void()> DoneFunc;
            typedef std::function<std::unique_ptr<Executor::Job::ThreadLocal>()> CreateThreadLocalFunc;
            
            RasterJob(int width, int height, int iterations, CreateThreadLocalFunc createThreadLocalFunc, ExecuteFunc executeFunc, DoneFunc doneFunc = DoneFunc());

            std::unique_ptr<Executor::Job::ThreadLocal> createThreadLocal() override;
            bool execute(Executor::Job::ThreadLocal &threadLocal) override;
            void done() override;

        private:
            int mWidth;
            int mHeight;
            int mIterations;
            CreateThreadLocalFunc mCreateThreadLocalFunc;
            ExecuteFunc mExecuteFunc;
            DoneFunc mDoneFunc;
            std::atomic_uint64_t mPixelIndex;
        };
    }
}
#endif