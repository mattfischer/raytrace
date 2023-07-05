#include "Render/Gpu/Renderer.hpp"

#include "Render/Gpu/WorkQueue.hpp"
#include "Render/Gpu/CLProxies.hpp"

#include "Math/Sampler/Random.hpp"

#include <memory>

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
        , mClRunIterationKernel(mClProgram, "runIteration", mClAllocator)
        {
            mRunning = false;

            mClAllocator.mapAreas();
            mContextProxy = mClAllocator.allocate<ContextProxy>();

            scene.writeProxy(mContextProxy->scene, mClAllocator);;
            mContextProxy->settings.width = mSettings.width;
            mContextProxy->settings.height = mSettings.height;
            mContextProxy->settings.minSamples = mSettings.minSamples;

            mContextProxy->items = mClAllocator.allocateArray<ItemProxy>(kSize);
            mContextProxy->random = mClAllocator.allocateArray<float>(kSize * 11);

            mClAllocator.unmapAreas();
            mClRunIterationKernel.setArg(0, mContextProxy);

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

        Renderer::~Renderer()
        {
            stop();
        }

        std::vector<std::string> Renderer::getSourceList()
        {
            return std::vector<std::string> {
                "Math/CLKernels.cl",
                "Object/Albedo/CLKernels.cl",
                "Object/Brdf/CLKernels.cl",
                "Object/Shape/CLKernels.cl",
                "Object/CLKernels.cl",
                "Render/Gpu/CLKernels.cl"
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

            if(mThread.joinable()) {
                mThread.join();
            }
            mThread = std::thread([&]() { runThread(); });
        }

        void Renderer::stop()
        {
            if(mRunning) {
                mRunning = false;
                mThread.join();
            }
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

        static Math::Color toneMap(const Math::Radiance &radiance)
        {
            float red = radiance.red() / (radiance.red() + 1);
            float green = radiance.green() / (radiance.green() + 1);
            float blue = radiance.blue() / (radiance.blue() + 1);

            return Math::Color(red, green, blue);
        }

        void Renderer::runThread()
        {
            while(mRunning) {
                mClAllocator.mapAreas();
                for(int i=0; i<kSize * 11; i++) {
                    mContextProxy->random[i] = mSampler.getValue();
                }
                mClAllocator.unmapAreas();

                mClRunIterationKernel.enqueue(mClContext, 1);
                clFlush(mClContext.clQueue());

                mClAllocator.mapAreas();
                int numQueued = mCommitRadianceQueue->numQueued();
                for(int i=0; i<numQueued; i++) {
                    WorkQueue::Key key = mCommitRadianceQueue->getNextKey();
                    ItemProxy &item = mContextProxy->items[key];

                    Math::Radiance radTotal = mTotalRadiance.get(item.x, item.y) + Math::Radiance(item.radiance);
                    mTotalRadiance.set(item.x, item.y, radTotal);

                    int numSamples = mTotalSamples.get(item.x, item.y) + 1;
                    Math::Color color = toneMap(radTotal / static_cast<float>(numSamples));
                    mRenderFramebuffer->setPixel(item.x, item.y, color);

                    mTotalSamples.set(item.x, item.y, numSamples);

                    mGenerateCameraRayQueue->addItem(key);
                }
                mCommitRadianceQueue->clear();

                int numRays = mGenerateCameraRayQueue->numQueued() + mIntersectRayQueue->numQueued();
                mClAllocator.unmapAreas();

                if(numRays == 0) {
                    auto endTime = std::chrono::steady_clock::now();
                    std::chrono::duration<double> duration = endTime - mStartTime;
                    mListener->onRendererDone(duration.count()); 
                    mRunning = false;
                    break;
                }

            }    
        }
    }
}