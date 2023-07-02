#include "Render/Gpu/Renderer.hpp"

#include "Render/Gpu/WorkQueue.hpp"

#include "Math/Sampler/Random.hpp"
#include <memory>

#include "Proxies.hpp"

using namespace std::placeholders;

namespace Render {
    namespace Gpu {
        static const int kSize = 10000;

        Renderer::Renderer(const Object::Scene &scene, const Settings &settings)
        : mScene(scene)
        , mSettings(settings)
        , mTotalRadiance(settings.width, settings.height)
        , mTotalSamples(settings.width, settings.height)
        , mClAllocator(mClContext)
        , mClProgram(mClContext, getSourceList())
        , mClGenerateCameraRayKernel(mClProgram, "generateCameraRays", mClAllocator)
        , mClIntersectRayKernel(mClProgram, "intersectRays", mClAllocator)
        , mClDirectLightAreaKernel(mClProgram, "directLightArea", mClAllocator)
        , mClDirectLightPointKernel(mClProgram, "directLightPoint", mClAllocator)
        , mClExtendPathKernel(mClProgram, "extendPath", mClAllocator)
        {
            mRunning = false;

            mClAllocator.mapAreas();
            mContextProxy = mClAllocator.allocate<ContextProxy>();

            scene.writeProxy(mContextProxy->scene, mClAllocator);;
            mContextProxy->settings.width = mSettings.width;
            mContextProxy->settings.height = mSettings.height;
            mContextProxy->settings.minSamples = mSettings.minSamples;

            mContextProxy->items = (ItemProxy*)mClAllocator.allocateBytes(sizeof(ItemProxy) * kSize);
            mContextProxy->random = (float*)mClAllocator.allocateBytes(sizeof(float) * kSize * 10);

            mClAllocator.unmapAreas();
            mClGenerateCameraRayKernel.setArg(0, mContextProxy);
            mClIntersectRayKernel.setArg(0, mContextProxy);
            mClDirectLightAreaKernel.setArg(0, mContextProxy);
            mClDirectLightPointKernel.setArg(0, mContextProxy);
            mClExtendPathKernel.setArg(0, mContextProxy);

            mRenderFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);
            mSampleStatusFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);
                        
            mGenerateCameraRayQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mIntersectRayQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mDirectLightAreaQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mDirectLightPointQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mExtendPathQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mCommitRadianceQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
        
            mGenerateCameraRayQueue->writeProxy(mContextProxy->generateCameraRayQueue);
            mIntersectRayQueue->writeProxy(mContextProxy->intersectRaysQueue);
            mDirectLightAreaQueue->writeProxy(mContextProxy->directLightAreaQueue);
            mDirectLightPointQueue->writeProxy(mContextProxy->directLightPointQueue);
            mExtendPathQueue->writeProxy(mContextProxy->extendPathQueue);
            mCommitRadianceQueue->writeProxy(mContextProxy->commitRadianceQueue);
        }

        std::vector<std::string> Renderer::getSourceList()
        {
            return std::vector<std::string> {
                "Math/Kernels.cl",
                "Object/Shape/Kernels.cl",
                "Object/Kernels.cl",
                "Render/Gpu/Kernels.cl"
            };
        }

        void Renderer::start(Listener *listener)
        {
            mListener = listener;
            mRunning = true;
            mStartTime = std::chrono::steady_clock::now();

            mClAllocator.mapAreas();
            mContextProxy->currentPixel = 0;
            for(WorkQueue::Key key = 0; key < kSize; key++) {
                mGenerateCameraRayQueue->addItem(key);
            }
            mClAllocator.unmapAreas();

            mThread = std::thread([&]() { runThread(); });
        }

        void Renderer::stop()
        {
            mRunning = false;
        }

        bool Renderer::running()
        {
            return mRunning;
        }

        Render::Framebuffer &Renderer::renderFramebuffer()
        {
            return *mRenderFramebuffer;
        }

        Render::Framebuffer &Renderer::sampleStatusFramebuffer()
        {
            return *mSampleStatusFramebuffer;
        }

        static Object::Color toneMap(const Object::Radiance &radiance)
        {
            float red = radiance.red() / (radiance.red() + 1);
            float green = radiance.green() / (radiance.green() + 1);
            float blue = radiance.blue() / (radiance.blue() + 1);

            return Object::Color(red, green, blue);
        }

        void Renderer::runGenerateCameraRays()
        {
            mClAllocator.mapAreas();
            int numQueued = mGenerateCameraRayQueue->numQueued();
            mClAllocator.unmapAreas();

            mClGenerateCameraRayKernel.enqueue(mClContext, numQueued);
            clFlush(mClContext.clQueue());

            mClAllocator.mapAreas();
            mGenerateCameraRayQueue->clear();
            mClAllocator.unmapAreas();
        }

        void Renderer::runIntersectRays()
        {
            mClAllocator.mapAreas();
            int numQueued = mIntersectRayQueue->numQueued();
            mClAllocator.unmapAreas();

            mClIntersectRayKernel.enqueue(mClContext, numQueued);
            clFlush(mClContext.clQueue());

            mClAllocator.mapAreas();
            mIntersectRayQueue->clear();
            mClAllocator.unmapAreas();
        }

        void Renderer::runDirectLightArea()
        {
            mClAllocator.mapAreas();
            int numQueued = mDirectLightAreaQueue->numQueued();
            mClAllocator.unmapAreas();

            mClDirectLightAreaKernel.enqueue(mClContext, numQueued);
            clFlush(mClContext.clQueue());

            mClAllocator.mapAreas();
            mDirectLightAreaQueue->clear();
            mClAllocator.unmapAreas();
        }

        void Renderer::runDirectLightPoint()
        {
            mClAllocator.mapAreas();
            int numQueued = mDirectLightPointQueue->numQueued();
            mClAllocator.unmapAreas();

            mClDirectLightPointKernel.enqueue(mClContext, numQueued);
            clFlush(mClContext.clQueue());

            mClAllocator.mapAreas();
            mDirectLightPointQueue->clear();
            mClAllocator.unmapAreas();
        }

        void Renderer::runExtendPath()
        {
            mClAllocator.mapAreas();
            int numQueued = mExtendPathQueue->numQueued();
            mClAllocator.unmapAreas();

            mClExtendPathKernel.enqueue(mClContext, numQueued);
            clFlush(mClContext.clQueue());

            mClAllocator.mapAreas();
            mExtendPathQueue->clear();
            mClAllocator.unmapAreas();
        }

        void Renderer::runCommitRadiance()
        {
            mClAllocator.mapAreas();

            int numQueued = mCommitRadianceQueue->numQueued();
            for(int i=0; i<numQueued; i++) {
                WorkQueue::Key key = mCommitRadianceQueue->getNextKey();
                ItemProxy &item = mContextProxy->items[key];

                Object::Radiance radTotal = mTotalRadiance.get(item.x, item.y) + Object::Radiance(item.radiance);
                mTotalRadiance.set(item.x, item.y, radTotal);

                int numSamples = mTotalSamples.get(item.x, item.y) + 1;
                Object::Color color = toneMap(radTotal / static_cast<float>(numSamples));
                mRenderFramebuffer->setPixel(item.x, item.y, color);

                mTotalSamples.set(item.x, item.y, numSamples);

                mGenerateCameraRayQueue->addItem(key);
            }
            mCommitRadianceQueue->clear();
            mClAllocator.unmapAreas();
        }

        void Renderer::runThread()
        {
            while(mRunning) {
                mClAllocator.mapAreas();
                for(int i=0; i<kSize * 10; i++) {
                    mContextProxy->random[i] = mSampler.getValue();
                }
                mClAllocator.unmapAreas();

                runGenerateCameraRays();

                mClAllocator.mapAreas();
                int numIntersect = mIntersectRayQueue->numQueued();
                mClAllocator.unmapAreas();

                if(numIntersect == 0) {
                    auto endTime = std::chrono::steady_clock::now();
                    std::chrono::duration<double> duration = endTime - mStartTime;
                    mListener->onRendererDone(duration.count()); 
                    mRunning = false;
                    break;
                }

                runIntersectRays();
                runDirectLightArea();
                runDirectLightPoint();
                runExtendPath();
                runCommitRadiance();
            }    
        }
    }
}