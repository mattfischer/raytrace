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
                        
            mGenerateCameraRayQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize);
            mGenerateCameraRayJob = std::make_unique<Executor::FuncJob<ThreadLocal>>(
                [&](ThreadLocal &threadLocal) {
                    return generateCameraRay(threadLocal);
                },
                [&]() {
                    mGenerateCameraRayQueue->clear();
                    if(mIntersectRayQueue->numQueued() > 0) {
                        runIntersectRays();
                    } else {
                        auto endTime = std::chrono::steady_clock::now();
                        std::chrono::duration<double> duration = endTime - mStartTime;
                        mListener->onRendererDone(duration.count()); 
                        mRunning = false;
                    }
                }
            );

            mIntersectRayQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize);
            mIntersectRayJob = std::make_unique<Executor::FuncJob<ThreadLocal>>(
                [&](ThreadLocal &threadLocal) {
                    return intersectRay(threadLocal);
                },
                [&]() {
                    mIntersectRayQueue->clear();
                    runDirectLightArea();
                }
            );

            mDirectLightAreaQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize);
            mDirectLightAreaJob = std::make_unique<Executor::FuncJob<ThreadLocal>>(
                [&](ThreadLocal &threadLocal) {
                    return directLightArea(threadLocal);
                },
                [&]() {
                    mDirectLightAreaQueue->clear();
                    mExecutor.runJob(*mDirectLightPointJob);
                }
            );

            mDirectLightPointQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize);
            mDirectLightPointJob = std::make_unique<Executor::FuncJob<ThreadLocal>>(
                [&](ThreadLocal &threadLocal) {
                    return directLightPoint(threadLocal);
                },
                [&]() {
                    mDirectLightPointQueue->clear();
                    mExecutor.runJob(*mExtendPathJob);
                }
            );

            mExtendPathQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize);
            mExtendPathJob = std::make_unique<Executor::FuncJob<ThreadLocal>>(
                [&](ThreadLocal &threadLocal) {
                    return extendPath(threadLocal);
                },
                [&]() {
                    mExtendPathQueue->clear();
                    mExecutor.runJob(*mCommitRadianceJob);
                }
            );

            mCommitRadianceQueue = std::make_unique<Render::Gpu::WorkQueue>(kSize);
            mCommitRadianceJob = std::make_unique<Executor::FuncJob<ThreadLocal>>(
                [&](ThreadLocal &threadLocal) {
                    return commitRadiance(threadLocal);
                },
                [&]() {
                    mCommitRadianceQueue->clear();
                    runGenerateCameraRays();
                }
            );
        }

        void Renderer::start(Listener *listener)
        {
            mListener = listener;
            mRunning = true;
            mStartTime = std::chrono::steady_clock::now();

            for(WorkQueue::Key key = 0; key < kSize; key++) {
                mGenerateCameraRayQueue->addItem(key);
            }

            runGenerateCameraRays();
        }

        void Renderer::stop()
        {
            mRunning = false;
            mExecutor.stop();
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

        bool Renderer::generateCameraRay(ThreadLocal &threadLocal)
        {
            WorkQueue::Key key = mGenerateCameraRayQueue->getNextKey();
            if(key == WorkQueue::kInvalidKey) {
                return false;
            }

            Item &item = mItems[key];

            unsigned int currentPixel = mCurrentPixel++;

            int sample = currentPixel / (mSettings.width * mSettings.height);
        
            if(sample >= mSettings.minSamples) {
                return false;
            }

            item.y = (currentPixel / mSettings.width) % mSettings.height;
            item.x = currentPixel % mSettings.width;

            Math::Bivector dv;

            threadLocal.sampler.startSample();

            Math::Point2D imagePoint = Math::Point2D((float)item.x, (float)item.y) + threadLocal.sampler.getValue2D();
            Math::Point2D aperturePoint = threadLocal.sampler.getValue2D();
            item.beam = mScene.camera().createPixelBeam(imagePoint, mSettings.width, mSettings.height, aperturePoint);
            item.specularBounce = false;
            item.generation = 0;
            item.radiance = Object::Radiance();
            item.throughput = Object::Color(1, 1, 1);

            mIntersectRayQueue->addItem(key);

            return true;
        }

        bool Renderer::intersectRay(ThreadLocal &threadLocal)
        {
            WorkQueue::Key key = mIntersectRayQueue->getNextKey();
            if(key == WorkQueue::kInvalidKey) {
                return false;
            }

            Item &item = mItems[key];

            item.isect = mScene.intersect(item.beam);

            Object::Intersection &isect = item.isect;
            if(isect.valid()) {
                const Object::Primitive &primitive = isect.primitive();
                Object::Radiance rad2 = primitive.surface().radiance();
                float misWeight = 1.0f;
                if(rad2.magnitude() > 0 && !item.specularBounce && item.generation > 0) {
                    float dot2 = -isect.facingNormal() * isect.ray().direction();
                    float pdfArea = item.pdf * dot2 / (isect.distance() * isect.distance());
                    float pdfLight = primitive.shape().samplePdf(isect.point());
                    misWeight = pdfArea * pdfArea / (pdfArea * pdfArea + pdfLight * pdfLight);
                }

                int totalLights = mScene.areaLights().size() + mScene.pointLights().size();
                int lightIndex = (int)std::floor(threadLocal.sampler.getValue() * totalLights);

                item.radiance += rad2 * item.throughput * misWeight;

                if(lightIndex < mScene.areaLights().size()) {
                    item.lightIndex = lightIndex;
                    mDirectLightAreaQueue->addItem(key);
                } else {
                    item.lightIndex = lightIndex - mScene.areaLights().size();
                    mDirectLightPointQueue->addItem(key);
                }
            } else {
                Object::Radiance rad2 = mScene.skyRadiance();
                item.radiance += rad2 * item.throughput;

                mCommitRadianceQueue->addItem(key);
            }

            return true;
        }

        bool Renderer::directLightArea(ThreadLocal &threadLocal)
        {
            WorkQueue::Key key = mDirectLightAreaQueue->getNextKey();
            if(key == WorkQueue::kInvalidKey) {
                return false;
            }

            Item &item = mItems[key];

            const Object::Intersection &isect = item.isect;
            const Object::Surface &surface = isect.primitive().surface();
            const Math::Normal &nrmFacing = isect.facingNormal();
            Math::Point pntOffset = item.isect.point() + Math::Vector(nrmFacing) * 0.01f;
            Object::Radiance rad;

            const Object::Primitive &light = mScene.areaLights()[item.lightIndex];
            const Object::Radiance &rad2 = light.surface().radiance();
                
            Math::Point pnt2;
            Math::Normal nrm2;
            float pdf;
            float misWeight = 1.0f;
            if(light.shape().sample(threadLocal.sampler, pnt2, nrm2, pdf)) {
                Math::Vector dirIn = pnt2 - pntOffset;
                float d = dirIn.magnitude();
                dirIn = dirIn / d;
                float dot2 = std::abs(dirIn * nrm2);

                float dot = dirIn * nrmFacing;
                if(dot > 0) {
                    Math::Ray ray(pntOffset, dirIn);
                    Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                    Object::Intersection isect2 = mScene.intersect(beam);

                    if (isect2.valid() && &(isect2.primitive()) == &light) {
                        Object::Radiance irad = rad2 * dot2 * dot / (d * d * pdf);
                        float pdfBrdf = surface.pdf(isect, dirIn) * dot2 / (d * d);
                        float misWeight = pdf * pdf / (pdf * pdf + pdfBrdf * pdfBrdf);
                        rad = irad * surface.reflected(isect, dirIn);
                        item.radiance += rad * item.throughput * misWeight;
                    }
                }
            }

            mExtendPathQueue->addItem(key);

            return true;
        }

        bool Renderer::directLightPoint(ThreadLocal &threadLocal)
        {
            WorkQueue::Key key = mDirectLightPointQueue->getNextKey();
            if(key == WorkQueue::kInvalidKey) {
                return false;
            }

            Item &item = mItems[key];

            const Object::Intersection &isect = item.isect;
            const Object::Surface &surface = isect.primitive().surface();
            const Math::Normal &nrmFacing = isect.facingNormal();
            Math::Point pntOffset = item.isect.point() + Math::Vector(nrmFacing) * 0.01f;
            Object::Radiance rad;

            const Object::PointLight &pointLight = *mScene.pointLights()[item.lightIndex];
        
            Math::Vector dirIn = pointLight.position() - pntOffset;
            float d = dirIn.magnitude();
            dirIn = dirIn / d;

            float dot = dirIn * nrmFacing;
            if(dot > 0) {
                Math::Ray ray(pntOffset, dirIn);
                Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                Object::Intersection isect2 = mScene.intersect(beam);

                if (!isect2.valid() || isect2.distance() >= d) {
                    Object::Radiance irad = pointLight.radiance() * dot / (d * d);
                    rad = irad * surface.reflected(isect, dirIn);
                    item.radiance += rad * item.throughput;
                }
            }

            item.radiance += rad * item.throughput;
            mExtendPathQueue->addItem(key);

            return true;
        }
        
        bool Renderer::extendPath(ThreadLocal &threadLocal)
        {
            WorkQueue::Key key = mExtendPathQueue->getNextKey();
            if(key == WorkQueue::kInvalidKey) {
                return false;
            }

            Item &item = mItems[key];

            const Object::Intersection &isect = item.isect;
            const Object::Surface &surface = isect.primitive().surface();
            const Object::Scene &scene = isect.scene();
            const Math::Normal &nrmFacing = isect.facingNormal();
            const Math::Vector dirOut = -isect.ray().direction();
            
            Object::Radiance rad = isect.primitive().surface().radiance();

            Math::Vector dirIn;
            Object::Color reflected = surface.sample(isect, threadLocal.sampler, dirIn, item.pdf, item.specularBounce);
            float reverse = (dirIn * nrmFacing > 0) ? 1.0f : -1.0f;
            float dot = dirIn * nrmFacing * reverse;

            Math::Point pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f * reverse;

            if(dot > 0) {
                item.throughput = item.throughput * reflected * dot / item.pdf;

                float threshold = 0.0f;
                float roulette = threadLocal.sampler.getValue();
                if(item.generation == 0) {
                    threshold = 1.0f;
                } else if(item.generation < 10) {
                    threshold = std::min(1.0f, item.throughput.maximum());
                }

                if(roulette < threshold) {            
                    Math::Ray reflectRay(pntOffset, dirIn);
                    item.beam = Math::Beam(reflectRay, Math::Bivector(), Math::Bivector());
                    item.throughput = item.throughput / threshold;
                    item.generation++;
                    mIntersectRayQueue->addItem(key);
                } else {
                    mCommitRadianceQueue->addItem(key);
                }
            } else {
                mCommitRadianceQueue->addItem(key);
            }

            return true;
        }

        static Object::Color toneMap(const Object::Radiance &radiance)
        {
            float red = radiance.red() / (radiance.red() + 1);
            float green = radiance.green() / (radiance.green() + 1);
            float blue = radiance.blue() / (radiance.blue() + 1);

            return Object::Color(red, green, blue);
        }

        bool Renderer::commitRadiance(ThreadLocal &threadLocal)
        {
            WorkQueue::Key key = mCommitRadianceQueue->getNextKey();
            if(key == WorkQueue::kInvalidKey) {
                return false;
            }

            Item &item = mItems[key];

            std::lock_guard<std::mutex> lock(mFramebufferMutex);
            Object::Radiance radTotal = mTotalRadiance.get(item.x, item.y) + item.radiance;
            mTotalRadiance.set(item.x, item.y, radTotal);

            int numSamples = mTotalSamples.get(item.x, item.y) + 1;
            Object::Color color = toneMap(radTotal / static_cast<float>(numSamples));
            mRenderFramebuffer->setPixel(item.x, item.y, color);

            mTotalSamples.set(item.x, item.y, numSamples);

            mGenerateCameraRayQueue->addItem(key);

            return true;
        }

        void Renderer::runGenerateCameraRays()
        {
            int numQueued = mGenerateCameraRayQueue->numQueued();

            mClAllocator.mapAreas();
            for(int i=0; i<kSize * 10; i++) {
                mRandom[i] = mThreadLocal.sampler.getValue();
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
            mClAllocator.unmapAreas();

            mGenerateCameraRayQueue->clear();
            if(mIntersectRayQueue->numQueued() > 0) {
                runIntersectRays();
            } else {
                auto endTime = std::chrono::steady_clock::now();
                std::chrono::duration<double> duration = endTime - mStartTime;
                mListener->onRendererDone(duration.count()); 
                mRunning = false;
            }
        }

        void Renderer::runIntersectRays()
        {
            int numQueued = mIntersectRayQueue->numQueued();

            mClAllocator.mapAreas();
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

                if(item.isect.valid()) {
                    int lightIndex = mItemProxies[i].lightIndex;
                    if(lightIndex < mScene.areaLights().size()) {
                        item.lightIndex = lightIndex;
                        mDirectLightAreaQueue->addItem(key);
                    } else {
                        item.lightIndex = lightIndex - mScene.areaLights().size();
                        mDirectLightPointQueue->addItem(key);
                    }
                } else {
                    mCommitRadianceQueue->addItem(key);
                }
            }
            mClAllocator.unmapAreas();

            mIntersectRayQueue->clear();
            runDirectLightArea();
        }

        void Renderer::runDirectLightArea()
        {
            int numQueued = mDirectLightAreaQueue->numQueued();

            mClAllocator.mapAreas();
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
            mClAllocator.unmapAreas();

            mDirectLightAreaQueue->clear();
            runDirectLightPoint();   
        }

        void Renderer::runDirectLightPoint()
        {
            int numQueued = mDirectLightPointQueue->numQueued();

            mClAllocator.mapAreas();
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

            mClDirectLightPointKernel.enqueue(mClContext, numQueued);
            clFlush(mClContext.clQueue());

            mClAllocator.mapAreas();
            for(int i=0; i<numQueued; i++) {
                WorkQueue::Key key = mDirectLightPointQueue->getNextKey();
                Item &item = mItems[key];

                item.radiance += Object::Radiance(mItemProxies[i].radiance);
                mExtendPathQueue->addItem(key);
            }
            mClAllocator.unmapAreas();

            mDirectLightPointQueue->clear();
            runExtendPath();   
        }

        void Renderer::runExtendPath()
        {
            int numQueued = mExtendPathQueue->numQueued();

            mClAllocator.mapAreas();
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
            mClAllocator.unmapAreas();

            mExtendPathQueue->clear();
            mExecutor.runJob(*mCommitRadianceJob);
        }
    }
}