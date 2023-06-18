#include "Render/Queued/Renderer.hpp"

#include "Render/Queued/WorkQueue.hpp"
#include "Render/Queued/WorkQueueJob.hpp"

#include "Math/Sampler/Random.hpp"
#include <memory>

using namespace std::placeholders;

namespace Render {
    namespace Queued {
        static const int kSize = 10000;

        Renderer::Renderer(const Object::Scene &scene, const Settings &settings)
        : mScene(scene)
        , mSettings(settings)
        , mItems(kSize)
        , mTotalRadiance(settings.width, settings.height)
        , mTotalSamples(settings.width, settings.height)
        {
            mCurrentPixel = 0;
            mRunning = false;

            mRenderFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);
            mSampleStatusFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);
            
            auto createThreadLocalFunc = []() { return std::make_unique<ThreadLocal>(); };
            
            mGenerateCameraRayQueue = std::make_unique<Render::Queued::WorkQueue>(kSize + 1, [&](auto k, auto &l) { generateCameraRay(k, l); });
            mGenerateCameraRayJob = std::make_unique<WorkQueueJob>(*mGenerateCameraRayQueue, createThreadLocalFunc);

            mIntersectRayQueue = std::make_unique<Render::Queued::WorkQueue>(kSize + 1, [&](auto k, auto &l) { intersectRay(k, l); });
            mIntersectRayJob = std::make_unique<WorkQueueJob>(*mIntersectRayQueue, createThreadLocalFunc);

            mDirectLightAreaQueue = std::make_unique<Render::Queued::WorkQueue>(kSize + 1, [&](auto k, auto &l) { directLightArea(k, l); });
            mDirectLightAreaJob = std::make_unique<WorkQueueJob>(*mDirectLightAreaQueue, createThreadLocalFunc);

            mDirectLightPointQueue = std::make_unique<Render::Queued::WorkQueue>(kSize + 1, [&](auto k, auto &l) { directLightPoint(k, l); });
            mDirectLightPointJob = std::make_unique<WorkQueueJob>(*mDirectLightPointQueue, createThreadLocalFunc);

            mExtendPathQueue = std::make_unique<Render::Queued::WorkQueue>(kSize + 1, [&](auto k, auto &l) { extendPath(k, l); });
            mExtendPathJob = std::make_unique<WorkQueueJob>(*mExtendPathQueue, createThreadLocalFunc);

            mCommitRadianceQueue = std::make_unique<Render::Queued::WorkQueue>(kSize + 1, [&](auto k, auto &l) { commitRadiance(k, l); });
            mCommitRadianceJob = std::make_unique<WorkQueueJob>(*mCommitRadianceQueue, createThreadLocalFunc);
        }

        void Renderer::start(Listener *listener)
        {
            mListener = listener;
            mRunning = true;
            mStartTime = std::chrono::steady_clock::now();

            for(WorkQueue::Key key = 0; key < kSize; key++) {
                mGenerateCameraRayQueue->addItem(key);
            }

            runGenerateCameraRayJob();
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

        std::unique_ptr<Renderer::ThreadLocal> Renderer::createThreadLocal()
        {
            return std::make_unique<ThreadLocal>();
        }

        void Renderer::generateCameraRay(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocalBase)
        {
            Item &item = mItems[key];
            ThreadLocal &threadLocal = static_cast<ThreadLocal&>(threadLocalBase);

            unsigned int currentPixel = mCurrentPixel++;

            int sample = currentPixel / (mSettings.width * mSettings.height);
        
            if(sample >= mSettings.minSamples) {
                return;
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
        }

        void Renderer::intersectRay(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocalBase)
        {
            Item &item = mItems[key];
            ThreadLocal &threadLocal = static_cast<ThreadLocal&>(threadLocalBase);

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
        }

        void Renderer::directLightArea(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocalBase)
        {
            Item &item = mItems[key];
            ThreadLocal &threadLocal = static_cast<ThreadLocal&>(threadLocalBase);

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
        }

        void Renderer::directLightPoint(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocalBase)
        {
            Item &item = mItems[key];
            ThreadLocal &threadLocal = static_cast<ThreadLocal&>(threadLocalBase);

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
        }
        
        void Renderer::extendPath(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocalBase)
        {
            Item &item = mItems[key];
            ThreadLocal &threadLocal = static_cast<ThreadLocal&>(threadLocalBase);

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
        }

        static Object::Color toneMap(const Object::Radiance &radiance)
        {
            float red = radiance.red() / (radiance.red() + 1);
            float green = radiance.green() / (radiance.green() + 1);
            float blue = radiance.blue() / (radiance.blue() + 1);

            return Object::Color(red, green, blue);
        }

        void Renderer::commitRadiance(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocalBase)
        {
            Item &item = mItems[key];
            ThreadLocal &threadLocal = static_cast<ThreadLocal&>(threadLocalBase);

            std::lock_guard<std::mutex> lock(mFramebufferMutex);
            Object::Radiance radTotal = mTotalRadiance.get(item.x, item.y) + item.radiance;
            mTotalRadiance.set(item.x, item.y, radTotal);

            int numSamples = mTotalSamples.get(item.x, item.y) + 1;
            Object::Color color = toneMap(radTotal / static_cast<float>(numSamples));
            mRenderFramebuffer->setPixel(item.x, item.y, color);

            mTotalSamples.set(item.x, item.y, numSamples);

            mGenerateCameraRayQueue->addItem(key);
        }

        void Renderer::runGenerateCameraRayJob()
        {
            mExecutor.runJob(*mGenerateCameraRayJob, [&]() {
                if(mIntersectRayQueue->numQueued() > 0) {
                    runIntersectRayJob();
                } else {
                    auto endTime = std::chrono::steady_clock::now();
                    std::chrono::duration<double> duration = endTime - mStartTime;
                    mListener->onRendererDone(duration.count()); 
                    mRunning = false;
                }
            });
        }

        void Renderer::runIntersectRayJob()
        {
            mExecutor.runJob(*mIntersectRayJob, [&]() { runDirectLightAreaJob(); });
        }
        
        void Renderer::runDirectLightAreaJob()
        {
            mExecutor.runJob(*mDirectLightAreaJob, [&]() { runDirectLightPointJob(); });
        }
        
        void Renderer::runDirectLightPointJob()
        {
            mExecutor.runJob(*mDirectLightPointJob, [&]() { runExtendPathJob(); });
        }
        
        void Renderer::runExtendPathJob()
        {
            mExecutor.runJob(*mExtendPathJob, [&]() { runCommitRadianceJob(); });
        }
        
        void Renderer::runCommitRadianceJob()
        {
            mExecutor.runJob(*mCommitRadianceJob, [&]() { runGenerateCameraRayJob(); });
        }
    }
}