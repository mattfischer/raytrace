#ifndef RENDER_QUEUED_RENDERER_HPP
#define RENDER_QUEUED_RENDERER_HPP

#include "Object/Scene.hpp"

#include "Render/Renderer.hpp"

#include "Render/Framebuffer.hpp"
#include "Render/Executor.hpp"
#include "Render/Raster.hpp"
#include "Render/Queued/WorkQueue.hpp"

#include "Math/Beam.hpp"
#include "Math/Sampler/Random.hpp"

#include <vector>
#include <mutex>

namespace Render {
    namespace Queued {
        class Renderer : public Render::Renderer {
        public:
            struct Settings
            {
                unsigned int width;
                unsigned int height;
                unsigned int minSamples;
                unsigned int maxSamples;
                float sampleThreshold;
            };

            Renderer(const Object::Scene &scene, const Settings &settings);
        
        private:
            struct Item {
                int x;
                int y;
                int generation;
                Math::Beam beam;
                Object::Intersection isect;

                int lightIndex;

                float pdf;
                bool specularBounce;

                Object::Radiance radiance;
                Object::Color throughput;
            };

            struct ThreadLocal : public WorkQueue::ThreadLocal
            {
                Math::Sampler::Random sampler;
            };

            std::unique_ptr<ThreadLocal> createThreadLocal();

            void generateCameraRay(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocal);
            void intersectRay(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocal);
            void directLightArea(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocal);
            void directLightPoint(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocal);
            void extendPath(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocal);
            void commitRadiance(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocal);
            
            const Object::Scene &mScene;
            const Settings mSettings;

            std::vector<Item> mItems;

            std::unique_ptr<WorkQueue> mGenerateCameraRayQueue;
            std::unique_ptr<WorkQueue> mIntersectRayQueue;
            std::unique_ptr<WorkQueue> mDirectLightAreaQueue;
            std::unique_ptr<WorkQueue> mDirectLightPointQueue;
            std::unique_ptr<WorkQueue> mExtendPathQueue;
            std::unique_ptr<WorkQueue> mCommitRadianceQueue;

            std::atomic_uint64_t mCurrentPixel;

            std::mutex mFramebufferMutex;
            Raster<Object::Radiance> mTotalRadiance;
            Raster<int> mTotalSamples;
        };
    }
}
#endif