#include "Render/Gpu/Renderer.hpp"

#include "Render/Gpu/WorkQueue.hpp"
#include "Render/Gpu/CLProxies.hpp"

#include "Math/Impl/Sampler/Halton.hpp"

#include <memory>

using namespace std::placeholders;

namespace Render::Gpu {
    static const int kSize = 1000000;

    Renderer::Renderer(const Object::Scene &scene, const Settings &settings)
    : mScene(scene)
    , mSettings(settings)
    , mTotalRadiance(settings.width, settings.height)
    , mTotalSamples(settings.width, settings.height)
    , mClConstAllocator(mClContext)
    , mClRwAllocator(mClContext)
    , mClProgram(mClContext, getSourceList())
    , mClGenerateCameraRaysKernel(mClProgram, "generateCameraRays", mClConstAllocator, mClRwAllocator)
    , mClIntersectRaysKernel(mClProgram, "intersectRays", mClConstAllocator, mClRwAllocator)
    , mClDirectLightAreaKernel(mClProgram, "directLightArea", mClConstAllocator, mClRwAllocator)
    , mClDirectLightPointKernel(mClProgram, "directLightPoint", mClConstAllocator, mClRwAllocator)
    , mClExtendPathKernel(mClProgram, "extendPath", mClConstAllocator, mClRwAllocator)
    {
        mRunning = false;

        mClRwAllocator.mapAreas();
        mClConstAllocator.mapAreas();
        mContextProxy = mClRwAllocator.allocate<ContextProxy>();

        scene.writeProxy(mContextProxy->scene, mClConstAllocator);;
        mContextProxy->settings.width = mSettings.width;
        mContextProxy->settings.height = mSettings.height;
        mContextProxy->settings.samples = mSettings.samples;

        Math::Impl::Sampler::Halton sampler(mSettings.width, mSettings.height);
        sampler.writeProxy(mContextProxy->sampler, mClConstAllocator);
        mContextProxy->items = mClRwAllocator.allocateArray<ItemProxy>(kSize);

        mClGenerateCameraRaysKernel.setArg(0, mContextProxy);
        mClIntersectRaysKernel.setArg(0, mContextProxy);
        mClDirectLightAreaKernel.setArg(0, mContextProxy);
        mClDirectLightPointKernel.setArg(0, mContextProxy);
        mClExtendPathKernel.setArg(0, mContextProxy);

        mRenderFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);
                    
        mGenerateCameraRayQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClRwAllocator);
        mIntersectRayQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClRwAllocator);
        mDirectLightAreaQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClRwAllocator);
        mDirectLightPointQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClRwAllocator);
        mExtendPathQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClRwAllocator);
        mCommitRadianceQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClRwAllocator);
    
        mGenerateCameraRayQueue->writeProxy(mContextProxy->generateCameraRayQueue);
        mIntersectRayQueue->writeProxy(mContextProxy->intersectRaysQueue);
        mDirectLightAreaQueue->writeProxy(mContextProxy->directLightAreaQueue);
        mDirectLightPointQueue->writeProxy(mContextProxy->directLightPointQueue);
        mExtendPathQueue->writeProxy(mContextProxy->extendPathQueue);
        mCommitRadianceQueue->writeProxy(mContextProxy->commitRadianceQueue);
    
        mClConstAllocator.unmapAreas();
        mClRwAllocator.unmapAreas();
    }

    Renderer::~Renderer()
    {
        stop();
    }

    std::vector<std::string> Renderer::getSourceList()
    {
        return std::vector<std::string> {
            "Math/CLKernels.cl",
            "Math/Sampler/CLKernels.cl",
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

        mClRwAllocator.mapAreas();
        mContextProxy->currentPixel = 0;
        for(WorkQueue::Key key = 0; key < kSize; key++) {
            mGenerateCameraRayQueue->addItem(key);
        }
        mClRwAllocator.unmapAreas();

        if(mThread.joinable()) {
            mThread.join();
        }
        mThread = std::thread([&]() { runThread(); });
    }

    void Renderer::stop()
    {
        mRunning = false;
        if(mThread.joinable()) {
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
            mClRwAllocator.mapAreas();
            int kernelSize = mGenerateCameraRayQueue->numQueued();
            if(kernelSize > 0) {
                mClRwAllocator.unmapAreas();
                mClGenerateCameraRaysKernel.enqueue(mClContext, kernelSize);
                clFinish(mClContext.clQueue());
                mClRwAllocator.mapAreas();
                mGenerateCameraRayQueue->clear();
            }

            kernelSize = mIntersectRayQueue->numQueued();
            if(kernelSize > 0) {
                mClRwAllocator.unmapAreas();
                mClIntersectRaysKernel.enqueue(mClContext, kernelSize);
                clFinish(mClContext.clQueue());
                mClRwAllocator.mapAreas();
                mIntersectRayQueue->clear();
            }

            kernelSize = mDirectLightAreaQueue->numQueued();                
            if(kernelSize > 0) {
                mClRwAllocator.unmapAreas();
                mClDirectLightAreaKernel.enqueue(mClContext, kernelSize);
                clFinish(mClContext.clQueue());
                mClRwAllocator.mapAreas();
                mDirectLightAreaQueue->clear();
            }

            kernelSize = mDirectLightPointQueue->numQueued();
            if(kernelSize > 0) {
                mClRwAllocator.unmapAreas();
                mClDirectLightPointKernel.enqueue(mClContext, kernelSize);
                clFinish(mClContext.clQueue());
                mClRwAllocator.mapAreas();
                mDirectLightPointQueue->clear();
            }

            kernelSize = mExtendPathQueue->numQueued();
            if(kernelSize > 0) {
                mClRwAllocator.unmapAreas();
                mClExtendPathKernel.enqueue(mClContext, kernelSize);
                clFinish(mClContext.clQueue());
                mClRwAllocator.mapAreas();
                mExtendPathQueue->clear();
            }

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
            mClRwAllocator.unmapAreas();

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