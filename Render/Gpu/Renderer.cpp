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
            mSceneProxy = scene.buildProxy(mClAllocator);
            mSettingsProxy = mClAllocator.allocate<SettingsProxy>();
            mSettingsProxy->width = mSettings.width;
            mSettingsProxy->height = mSettings.height;
            mSettingsProxy->minSamples = mSettings.minSamples;

            mItemProxies = (ItemProxy*)mClAllocator.allocateBytes(sizeof(ItemProxy) * kSize);
            mRandom = (float*)mClAllocator.allocateBytes(sizeof(float) * kSize * 10);
            mQueuesProxy = mClAllocator.allocate<QueuesProxy>();
            mCurrentPixel = mClAllocator.allocate<unsigned int>();

            mClAllocator.unmapAreas();
            mClGenerateCameraRayKernel.setArg(0, mSceneProxy);
            mClGenerateCameraRayKernel.setArg(1, mSettingsProxy);
            mClGenerateCameraRayKernel.setArg(2, mItemProxies);
            mClGenerateCameraRayKernel.setArg(3, mRandom);
            mClGenerateCameraRayKernel.setArg(4, mQueuesProxy);
            mClGenerateCameraRayKernel.setArg(5, mCurrentPixel);
            mClIntersectRayKernel.setArg(0, mSceneProxy);
            mClIntersectRayKernel.setArg(1, mItemProxies);
            mClIntersectRayKernel.setArg(2, mRandom);
            mClIntersectRayKernel.setArg(3, mQueuesProxy);
            mClDirectLightAreaKernel.setArg(0, mSceneProxy);
            mClDirectLightAreaKernel.setArg(1, mItemProxies);
            mClDirectLightAreaKernel.setArg(2, mRandom);
            mClDirectLightAreaKernel.setArg(3, mQueuesProxy);

            mClDirectLightPointKernel.setArg(0, mSceneProxy);
            mClDirectLightPointKernel.setArg(1, mItemProxies);
            mClDirectLightPointKernel.setArg(2, mQueuesProxy);

            mClExtendPathKernel.setArg(0, mSceneProxy);
            mClExtendPathKernel.setArg(1, mItemProxies);
            mClExtendPathKernel.setArg(2, mRandom);
            mClExtendPathKernel.setArg(3, mQueuesProxy);

            mRenderFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);
            mSampleStatusFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);
                        
            mGenerateCameraRayQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mIntersectRayQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mDirectLightAreaQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mDirectLightPointQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mExtendPathQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mCommitRadianceQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
        
            mQueuesProxy->generateCameraRayQueue.data = (int*)mGenerateCameraRayQueue->data();
            mQueuesProxy->intersectRaysQueue.data = (int*)mIntersectRayQueue->data();
            mQueuesProxy->directLightAreaQueue.data = (int*)mDirectLightAreaQueue->data();
            mQueuesProxy->directLightPointQueue.data = (int*)mDirectLightPointQueue->data();
            mQueuesProxy->extendPathQueue.data = (int*)mExtendPathQueue->data();
            mQueuesProxy->commitRadianceQueue.data = (int*)mCommitRadianceQueue->data();
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
            *mCurrentPixel = 0;
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
                ItemProxy &item = mItemProxies[key];

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
                    mRandom[i] = mSampler.getValue();
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