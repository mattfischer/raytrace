#ifndef RENDER_GPU_RENDERER_HPP
#define RENDER_GPU_RENDERER_HPP

#include "Object/Scene.hpp"

#include "Render/Renderer.hpp"

#include "Render/Framebuffer.hpp"
#include "Render/Raster.hpp"
#include "Render/Gpu/WorkQueue.hpp"

#include "Math/Beam.hpp"

#include "OpenCL.hpp"

#include <vector>
#include <mutex>

namespace Render {
    namespace Gpu {
        class Renderer : public Render::Renderer {
        public:
            struct Settings
            {
                unsigned int width;
                unsigned int height;
                unsigned int samples;
            };

            Renderer(const Object::Scene &scene, const Settings &settings);
            ~Renderer();

            void start(Listener *listener) override;
            void stop() override;
            bool running() override;

            Render::Framebuffer &renderFramebuffer() override;

        private:
            std::vector<std::string> getSourceList();

            void runThread();

            bool mRunning;
            Listener *mListener;
            std::chrono::time_point<std::chrono::steady_clock> mStartTime;
  
            const Object::Scene &mScene;
            const Settings mSettings;
            std::unique_ptr<Render::Framebuffer> mRenderFramebuffer;

            std::unique_ptr<WorkQueue> mGenerateCameraRayQueue;            
            std::unique_ptr<WorkQueue> mIntersectRayQueue;            
            std::unique_ptr<WorkQueue> mDirectLightAreaQueue;
            std::unique_ptr<WorkQueue> mDirectLightPointQueue;
            std::unique_ptr<WorkQueue> mExtendPathQueue;
            std::unique_ptr<WorkQueue> mCommitRadianceQueue;

            std::thread mThread;

            std::mutex mFramebufferMutex;
            Raster<Math::Radiance> mTotalRadiance;
            Raster<int> mTotalSamples;

            OpenCL::Context mClContext;
            OpenCL::Allocator mClConstAllocator;
            OpenCL::Allocator mClRwAllocator;
            OpenCL::Program mClProgram;

            OpenCL::Kernel mClGenerateCameraRaysKernel;
            OpenCL::Kernel mClIntersectRaysKernel;
            OpenCL::Kernel mClDirectLightAreaKernel;
            OpenCL::Kernel mClDirectLightPointKernel;
            OpenCL::Kernel mClExtendPathKernel;

            ContextProxy *mContextProxy;
        };
    }
}
#endif