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
        , mItems(kSize)
        , mTotalRadiance(settings.width, settings.height)
        , mTotalSamples(settings.width, settings.height)
        , mClAllocator(mClContext)
        , mClProgram(mClContext, "kernels.cl")
        , mClGenerateCameraRayKernel(mClProgram, "generateCameraRays", mClAllocator)
        , mClIntersectRayKernel(mClProgram, "intersectRays", mClAllocator)
        , mClDirectLightAreaKernel(mClProgram, "directLightArea", mClAllocator)
        , mClDirectLightPointKernel(mClProgram, "directLightPoint", mClAllocator)
        , mClExtendPathKernel(mClProgram, "extendPath", mClAllocator)
        {
            mCurrentPixel = 0;
            mRunning = false;

            mClAllocator.mapAreas();
            mSceneProxy = scene.buildProxy(mClAllocator);
            mSettingsProxy = mClAllocator.allocate<SettingsProxy>();
            mSettingsProxy->width = mSettings.width;
            mSettingsProxy->height = mSettings.height;

            mItemProxies = (ItemProxy*)mClAllocator.allocateBytes(sizeof(ItemProxy) * kSize);
            mRandom = (float*)mClAllocator.allocateBytes(sizeof(float) * kSize * 10);
            mClAllocator.unmapAreas();
            mClGenerateCameraRayKernel.setArg(0, mSceneProxy);
            mClGenerateCameraRayKernel.setArg(1, mSettingsProxy);
            mClGenerateCameraRayKernel.setArg(2, mItemProxies);
            mClGenerateCameraRayKernel.setArg(3, mRandom);
            mClIntersectRayKernel.setArg(0, mSceneProxy);
            mClIntersectRayKernel.setArg(1, mItemProxies);
            mClIntersectRayKernel.setArg(2, mRandom);
            mClDirectLightAreaKernel.setArg(0, mSceneProxy);
            mClDirectLightAreaKernel.setArg(1, mItemProxies);
            mClDirectLightAreaKernel.setArg(2, mRandom);
            mClDirectLightPointKernel.setArg(0, mSceneProxy);
            mClDirectLightPointKernel.setArg(1, mItemProxies);
            mClExtendPathKernel.setArg(0, mSceneProxy);
            mClExtendPathKernel.setArg(1, mItemProxies);
            mClExtendPathKernel.setArg(2, mRandom);

            mRenderFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);
            mSampleStatusFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);
                        
            mGenerateCameraRayQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mIntersectRayQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mDirectLightAreaQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mDirectLightPointQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mExtendPathQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
            mCommitRadianceQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize, mClAllocator);
        }

        void Renderer::start(Listener *listener)
        {
            mListener = listener;
            mRunning = true;
            mStartTime = std::chrono::steady_clock::now();

            mClAllocator.mapAreas();
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
            for(int i=0; i<kSize * 10; i++) {
                mRandom[i] = mSampler.getValue();
            }

            int numGenerated = 0;
            for(int i=0; i<numQueued; i++) {
                WorkQueue::Key key = mGenerateCameraRayQueue->getNextKey();
                Item &item = mItems[key];

                unsigned int currentPixel = mCurrentPixel++;

                int sample = currentPixel / (mSettings.width * mSettings.height);
        
                if(sample >= mSettings.minSamples) {
                    break;
                }

                mItemProxies[i].currentPixel = currentPixel;
                numGenerated++;
            }
            mGenerateCameraRayQueue->resetRead();
            mClAllocator.unmapAreas();

            mClGenerateCameraRayKernel.enqueue(mClContext, numGenerated);
            clFlush(mClContext.clQueue());

            mClAllocator.mapAreas();
            for(int i=0; i<numGenerated; i++) {
                WorkQueue::Key key = mGenerateCameraRayQueue->getNextKey();
                Item &item = mItems[key];
            
                item.x = mItemProxies[i].x;
                item.y = mItemProxies[i].y;
                item.beam = Math::Beam(mItemProxies[i].beam);
                item.specularBounce = mItemProxies[i].specularBounce;
                item.generation = mItemProxies[i].generation;
                item.radiance = Object::Radiance(mItemProxies[i].radiance);
                item.throughput = Object::Color(mItemProxies[i].throughput);

                mIntersectRayQueue->addItem(key);
            }

            mGenerateCameraRayQueue->clear();
            int numIntersect = mIntersectRayQueue->numQueued();

            mClAllocator.unmapAreas();
        }

        void Renderer::runIntersectRays()
        {
            mClAllocator.mapAreas();

            int numQueued = mIntersectRayQueue->numQueued();
            for(int i=0; i<numQueued; i++) {
                WorkQueue::Key key = mIntersectRayQueue->getNextKey();
                Item &item = mItems[key];
                item.beam.writeProxy(mItemProxies[i].beam);
                mItemProxies[i].generation = item.generation;
                mItemProxies[i].specularBounce = item.specularBounce;
                mItemProxies[i].pdf = item.pdf;
                item.throughput.writeProxy(mItemProxies[i].throughput);
                item.radiance.writeProxy(mItemProxies[i].radiance);
            }
            mIntersectRayQueue->resetRead();
            mClAllocator.unmapAreas();

            mClIntersectRayKernel.enqueue(mClContext, numQueued);
            clFlush(mClContext.clQueue());

            mClAllocator.mapAreas();
            for(int i=0; i<numQueued; i++) {
                WorkQueue::Key key = mIntersectRayQueue->getNextKey();
                Item &item = mItems[key];
            
                Object::Shape::Base::Intersection shapeIntersection;
                shapeIntersection.distance = mItemProxies[i].isect.shapeIntersection.distance;
                shapeIntersection.normal = Math::Normal(mItemProxies[i].isect.shapeIntersection.normal);
                Object::Primitive *primitive = nullptr;
                if(mItemProxies[i].isect.primitive) {
                    primitive = (Object::Primitive*)mItemProxies[i].isect.primitive->primitive;
                    item.isectPrimitiveProxy = mItemProxies[i].isect.primitive;
                }

                item.isect = Object::Intersection(mScene, *primitive, item.beam, shapeIntersection);
                item.radiance += Object::Radiance(mItemProxies[i].radiance);
                item.lightIndex = mItemProxies[i].lightIndex;

                if(mItemProxies[i].nextQueue == 0) {
                    mDirectLightAreaQueue->addItem(key);
                } else if(mItemProxies[i].nextQueue == 1) {
                    mDirectLightPointQueue->addItem(key);
                } else if(mItemProxies[i].nextQueue == 2) {
                    mCommitRadianceQueue->addItem(key);
                }
            }
            mIntersectRayQueue->clear();
            mClAllocator.unmapAreas();
        }

        void Renderer::runDirectLightArea()
        {
            mClAllocator.mapAreas();

            int numQueued = mDirectLightAreaQueue->numQueued();
            for(int i=0; i<numQueued; i++) {
                WorkQueue::Key key = mDirectLightAreaQueue->getNextKey();
                Item &item = mItems[key];

                mItemProxies[i].lightIndex = item.lightIndex;
    
                item.isect.writeProxy(mItemProxies[i].isect);
                mItemProxies[i].isect.primitive = item.isectPrimitiveProxy;
                item.isect.beam().writeProxy(mItemProxies[i].beam);
                mItemProxies[i].isect.beam = &mItemProxies[i].beam;
                item.throughput.writeProxy(mItemProxies[i].throughput);
            }
            mDirectLightAreaQueue->resetRead();
            mClAllocator.unmapAreas();

            mClDirectLightAreaKernel.enqueue(mClContext, numQueued);
            clFlush(mClContext.clQueue());

            mClAllocator.mapAreas();
            for(int i=0; i<numQueued; i++) {
                WorkQueue::Key key = mDirectLightAreaQueue->getNextKey();
                Item &item = mItems[key];

                item.radiance += Object::Radiance(mItemProxies[i].radiance);
                mExtendPathQueue->addItem(key);
            }
            mDirectLightAreaQueue->clear();
            mClAllocator.unmapAreas();
        }

        void Renderer::runDirectLightPoint()
        {
            mClAllocator.mapAreas();

            int numQueued = mDirectLightPointQueue->numQueued();
            for(int i=0; i<numQueued; i++) {
                WorkQueue::Key key = mDirectLightPointQueue->getNextKey();
                Item &item = mItems[key];

                mItemProxies[i].lightIndex = item.lightIndex;
    
                item.isect.writeProxy(mItemProxies[i].isect);
                mItemProxies[i].isect.primitive = item.isectPrimitiveProxy;
                item.isect.beam().writeProxy(mItemProxies[i].beam);
                mItemProxies[i].isect.beam = &mItemProxies[i].beam;
                item.throughput.writeProxy(mItemProxies[i].throughput);
            }
            mDirectLightPointQueue->resetRead();
            mClAllocator.unmapAreas();

            mClDirectLightPointKernel.enqueue(mClContext, numQueued);
            clFlush(mClContext.clQueue());

            mClAllocator.mapAreas();
            for(int i=0; i<numQueued; i++) {
                WorkQueue::Key key = mDirectLightPointQueue->getNextKey();
                Item &item = mItems[key];

                item.radiance += Object::Radiance(mItemProxies[i].radiance);
                mExtendPathQueue->addItem(key);
            }
            mDirectLightPointQueue->clear();
            mClAllocator.unmapAreas();
        }

        void Renderer::runExtendPath()
        {
            mClAllocator.mapAreas();

            int numQueued = mExtendPathQueue->numQueued();
            for(int i=0; i<numQueued; i++) {
                WorkQueue::Key key = mExtendPathQueue->getNextKey();
                Item &item = mItems[key];

                item.isect.writeProxy(mItemProxies[i].isect);
                mItemProxies[i].isect.primitive = item.isectPrimitiveProxy;
                item.isect.beam().writeProxy(mItemProxies[i].beam);
                mItemProxies[i].isect.beam = &mItemProxies[i].beam;
                item.throughput.writeProxy(mItemProxies[i].throughput);
                mItemProxies[i].generation = item.generation;
            }
            mExtendPathQueue->resetRead();
            mClAllocator.unmapAreas();

            mClExtendPathKernel.enqueue(mClContext, numQueued);
            clFlush(mClContext.clQueue());

            mClAllocator.mapAreas();
            for(int i=0; i<numQueued; i++) {
                WorkQueue::Key key = mExtendPathQueue->getNextKey();
                Item &item = mItems[key];

                item.beam = Math::Beam(mItemProxies[i].beam);
                item.throughput = Object::Color(mItemProxies[i].throughput);
                item.generation = mItemProxies[i].generation;
                item.specularBounce = mItemProxies[i].specularBounce;
                item.pdf = mItemProxies[i].pdf;
                if(mItemProxies[i].nextQueue == 0) {
                    mIntersectRayQueue->addItem(key);
                } else if(mItemProxies[i].nextQueue == 1) {
                    mCommitRadianceQueue->addItem(key);
                }
            }
            mExtendPathQueue->clear();
            mClAllocator.unmapAreas();
        }

        void Renderer::runCommitRadiance()
        {
            mClAllocator.mapAreas();

            int numQueued = mCommitRadianceQueue->numQueued();
            for(int i=0; i<numQueued; i++) {
                WorkQueue::Key key = mCommitRadianceQueue->getNextKey();
                Item &item = mItems[key];

                Object::Radiance radTotal = mTotalRadiance.get(item.x, item.y) + item.radiance;
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