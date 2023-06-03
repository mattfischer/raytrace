#include "Render/UniPathRenderer.hpp"

#include "Render/WorkQueue.hpp"
#include "Render/WorkQueueJob.hpp"

#include "Math/Sampler/Random.hpp"
#include <memory>

using namespace std::placeholders;

namespace Render {
    static const int kSize = 10000;

    UniPathRenderer::UniPathRenderer(const Object::Scene &scene, const Settings &settings)
    : mScene(scene)
    , mSettings(settings)
    , mItems(kSize)
    , mTotalRadiance(settings.width, settings.height)
    , mTotalSamples(settings.width, settings.height)
    {
        mCurrentPixel = 0;

        mRenderFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);
        mSampleStatusFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);
        
        std::unique_ptr<Render::WorkQueueJob> workQueueJob = std::make_unique<Render::WorkQueueJob>([]() { return std::make_unique<ThreadLocal>(); });

        mGenerateCameraRayQueue = std::make_unique<Render::WorkQueue>(kSize + 1, [&](auto k, auto &l) { generateCameraRay(k, l); });
        workQueueJob->addWorkQueue(*mGenerateCameraRayQueue);

        mIntersectRayQueue = std::make_unique<Render::WorkQueue>(kSize + 1, [&](auto k, auto &l) { intersectRay(k, l); });
        workQueueJob->addWorkQueue(*mIntersectRayQueue);

        mDirectLightAreaQueue = std::make_unique<Render::WorkQueue>(kSize + 1, [&](auto k, auto &l) { directLightArea(k, l); });
        workQueueJob->addWorkQueue(*mDirectLightAreaQueue);

        mDirectLightPointQueue = std::make_unique<Render::WorkQueue>(kSize + 1, [&](auto k, auto &l) { directLightPoint(k, l); });
        workQueueJob->addWorkQueue(*mDirectLightPointQueue);

        mExtendPathQueue = std::make_unique<Render::WorkQueue>(kSize + 1, [&](auto k, auto &l) { extendPath(k, l); });
        workQueueJob->addWorkQueue(*mExtendPathQueue);

        mCommitRadianceQueue = std::make_unique<Render::WorkQueue>(kSize + 1, [&](auto k, auto &l) { commitRadiance(k, l); });
        workQueueJob->addWorkQueue(*mCommitRadianceQueue);

        for(WorkQueue::Key key = 0; key < kSize; key++) {
            mGenerateCameraRayQueue->addItem(key);
        }

        mExecutor.addJob(std::move(workQueueJob));
    }

    Render::Executor &UniPathRenderer::executor()
    {
        return mExecutor;
    }

    Render::Framebuffer &UniPathRenderer::renderFramebuffer()
    {
        return *mRenderFramebuffer;
    }

    Render::Framebuffer &UniPathRenderer::sampleStatusFramebuffer()
    {
        return *mSampleStatusFramebuffer;
    }
    
    std::unique_ptr<UniPathRenderer::ThreadLocal> UniPathRenderer::createThreadLocal()
    {
        return std::make_unique<ThreadLocal>();
    }

    void UniPathRenderer::generateCameraRay(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocalBase)
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

    void UniPathRenderer::intersectRay(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocalBase)
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
                float dot2 = -primitive.surface().facingNormal(isect) * isect.ray().direction();
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

    void UniPathRenderer::directLightArea(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocalBase)
    {
        Item &item = mItems[key];
        ThreadLocal &threadLocal = static_cast<ThreadLocal&>(threadLocalBase);

        const Object::Intersection &isect = item.isect;
        const Object::Surface &surface = isect.primitive().surface();
        const Math::Normal &nrmFacing = surface.facingNormal(isect);
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

    void UniPathRenderer::directLightPoint(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocalBase)
    {
        Item &item = mItems[key];
        ThreadLocal &threadLocal = static_cast<ThreadLocal&>(threadLocalBase);

        const Object::Intersection &isect = item.isect;
        const Object::Surface &surface = isect.primitive().surface();
        const Math::Normal &nrmFacing = surface.facingNormal(isect);
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
    
    void UniPathRenderer::extendPath(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocalBase)
    {
        Item &item = mItems[key];
        ThreadLocal &threadLocal = static_cast<ThreadLocal&>(threadLocalBase);

        const Object::Intersection &isect = item.isect;
        const Object::Surface &surface = isect.primitive().surface();
        const Object::Scene &scene = isect.scene();
        const Math::Normal &nrmFacing = surface.facingNormal(isect);
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

    void UniPathRenderer::commitRadiance(WorkQueue::Key key, WorkQueue::ThreadLocal &threadLocalBase)
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
}