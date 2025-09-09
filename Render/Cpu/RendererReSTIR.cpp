#include "Render/Cpu/RendererReSTIR.hpp"
#include "Render/Cpu/RasterJob.hpp"

#include <atomic>
#include <mutex>

namespace Render::Cpu {
    RendererReSTIR::RendererReSTIR(const Object::Scene &scene, const Settings &settings)
    : mScene(scene)
    , mSettings(settings)
    , mDirectReservoirs(settings.width, settings.height)
    , mIndirectReservoirs(settings.width, settings.height)
    , mPrimaryHits(settings.width, settings.height)
    , mTotalRadiance(settings.width, settings.height)
    {
        mRenderFramebuffer = std::make_unique<Render::Framebuffer>(settings.width, settings.height);

        mIndirectLighter = std::make_unique<Render::Cpu::Impl::Lighter::UniPath>();
    }

    void RendererReSTIR::start(Listener *listener)
    {
        mListener = listener;
        mStartTime = std::chrono::steady_clock::now();

        mCurrentSample = 0;
        startInitialSampleJob();
    }

    void RendererReSTIR::stop()
    {
        mExecutor.stop();
    }

    bool RendererReSTIR::running()
    {
        return mExecutor.running();
    }

    Render::Framebuffer &RendererReSTIR::renderFramebuffer()
    {
        return *mRenderFramebuffer;
    }

    void RendererReSTIR::startInitialSampleJob()
    {
        std::unique_ptr<Executor::Job> job = 
            std::make_unique<RasterJob>(
                mSettings.width,
                mSettings.height,
                1,
                [&]() { return std::make_unique<ThreadLocal>(mRenderFramebuffer->width(), mRenderFramebuffer->height(), mSettings.indirectSamples); },
                [&](int x, int y, int sample, Executor::Job::ThreadLocal &threadLocalBase)
                    {
                        initialSamplePixel(x, y, mCurrentSample, static_cast<ThreadLocal&>(threadLocalBase).sampler);
                    },
                [&]() { startDirectIlluminateJob(); }
            );

        mExecutor.runJob(std::move(job));
    }

    void RendererReSTIR::startDirectIlluminateJob()
    {
        std::unique_ptr<Executor::Job> job = 
            std::make_unique<RasterJob>(
                mSettings.width,
                mSettings.height,
                1,
                [&]() { return std::make_unique<ThreadLocal>(mRenderFramebuffer->width(), mRenderFramebuffer->height(), mSettings.indirectSamples); },
                [&](int x, int y, int sample, Executor::Job::ThreadLocal &threadLocalBase)
                    {
                        directIlluminatePixel(x, y, mCurrentSample, static_cast<ThreadLocal&>(threadLocalBase).sampler);
                    },
                [&]() { startIndirectIlluminateJob(); }
            );

        mExecutor.runJob(std::move(job));
    }

    void RendererReSTIR::startIndirectIlluminateJob()
    {
        std::unique_ptr<Executor::Job> job = 
            std::make_unique<RasterJob>(
                mSettings.width,
                mSettings.height,
                1,
                [&]() { return std::make_unique<ThreadLocal>(mRenderFramebuffer->width(), mRenderFramebuffer->height(), mSettings.indirectSamples); },
                [&](int x, int y, int sample, Executor::Job::ThreadLocal &threadLocalBase)
                    {
                        indirectIlluminatePixel(x, y, mCurrentSample, static_cast<ThreadLocal&>(threadLocalBase).sampler, &static_cast<ThreadLocal&>(threadLocalBase).indirectSamples[0]);
                    },
                [&]() 
                    {
                        mCurrentSample++;
                        if(mCurrentSample < mSettings.samples) {
                            startInitialSampleJob();
                        } else {
                            auto endTime = std::chrono::steady_clock::now();
                            std::chrono::duration<double> duration = endTime - mStartTime;
                            mListener->onRendererDone(duration.count());
                        }
                    }
            );

        mExecutor.runJob(std::move(job));
    }

    void RendererReSTIR::initialSamplePixel(int x, int y, int sample, Math::Sampler &sampler)
    {
        Math::Bivector dv;
        sampler.startSample(x, y, sample);
        Math::Point2D imagePoint = Math::Point2D((float)x, (float)y) + sampler.getValue2D();
        Math::Point2D aperturePoint = sampler.getValue2D();
        Math::Beam beam = mScene.camera().createPixelBeam(imagePoint, mSettings.width, mSettings.height, aperturePoint);
        Object::Intersection isect = mScene.intersect(beam, FLT_MAX, true);

        PrimaryHit &primaryHit = mPrimaryHits.at(x, y);
        primaryHit.beam = beam;
        primaryHit.isect = Object::Intersection(mScene, isect.primitive(), primaryHit.beam, isect.shapeIntersection());

        const Math::Normal &nrmFacing = isect.facingNormal(); 
        const Object::Surface &surface = isect.primitive().surface();
        Math::Point pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f;

        Reservoir<DirectSample> &resDirect = mDirectReservoirs.at(x, y);
        resDirect.clear();

        Math::Radiance radEmitted;
        if (isect.valid()) {   
            for(int i=0; i<1; i++) {
                int lightIndex = (int)std::floor(sampler.getValue() * mScene.areaLights().size());
                const Object::Primitive &light = mScene.areaLights()[lightIndex];
                const Math::Radiance &rad2 = light.surface().radiance();

                auto [pnt2, nrm2, pdf2] = light.shape().sample(sampler);
                if(pdf2 > 0.0f) {
                    Math::Vector dirIn = pnt2 - pntOffset;
                    float d = dirIn.magnitude();
                    dirIn = dirIn / d;
                    float dot2 = std::abs(dirIn * nrm2);

                    float dot = dirIn * nrmFacing;
                    if(dot > 0) {
                        Math::Radiance irad = rad2 * dot2 * dot / (d * d);
                        Math::Radiance radDirect = irad * surface.reflected(isect, dirIn);
                        float q = radDirect.magnitude();

                        DirectSample sample;
                        sample.point = pnt2;
                        sample.radiance = rad2;
                        sample.normal = nrm2;
                        sample.primitive = &light;
                        
                        resDirect.addSample(sample, q, pdf2, sampler);
                    }
                }
            }

            Reservoir<IndirectSample> &resIndirect = mIndirectReservoirs.at(x, y);           
            resIndirect.clear();

            auto [reflected, dirIn, pdf] = surface.sample(isect, sampler);
            float reverse = (dirIn * nrmFacing > 0) ? 1.0f : -1.0f;
            float dot = dirIn * nrmFacing * reverse;

            Math::Point pntOffset = isect.point() + Math::Vector(nrmFacing) * 0.01f * reverse;

            if(dot > 0) {
                Math::Ray reflectRay(pntOffset, dirIn);
                Math::Beam beam(reflectRay, Math::Bivector(), Math::Bivector());
                Object::Intersection isect2 = mScene.intersect(beam, FLT_MAX, true);

                if (isect2.valid()) {
                    Math::Radiance rad2 = mIndirectLighter->light(isect2, sampler);
                
                    IndirectSample sample;
                    sample.point = isect2.point();
                    sample.normal = isect2.facingNormal();
                    sample.indirectRadiance = (rad2 - isect2.primitive().surface().radiance()); 
                    float q = sample.indirectRadiance.magnitude();
                    resIndirect.addSample(sample, q, pdf, sampler);
                }
            }

            radEmitted = isect.primitive().surface().radiance();
        } else {
            for(const Object::Light &light : mScene.skyLights()) {
                radEmitted += light.radiance(beam.ray().direction());
            }
        }

        addRadiance(x, y, sample, radEmitted);
    }

    void RendererReSTIR::directIlluminatePixel(int x, int y, int sample, Math::Sampler &sampler)
    {
        Math::Radiance radDirect;
        PrimaryHit &primaryHit = mPrimaryHits.at(x, y);
        const Math::Normal &nrmFacing = primaryHit.isect.facingNormal(); 
        const Object::Surface &surface = primaryHit.isect.primitive().surface();
        Math::Point pntOffset = primaryHit.isect.point() + Math::Vector(nrmFacing) * 0.01f;

        Reservoir<DirectSample> res;
        res.clear();

        const int R = mSettings.radius;
        for(int i=0; i<mSettings.candidates; i++) {
            Math::Point2D s = sampler.getValue2D();
            s = s * R * 2 + Math::Point2D(-R, -R);
            int sx = (int)std::floor(s.u() + x);
            int sy = (int)std::floor(s.v() + y);
            if(sx < 0 || sy < 0 || sx >= mSettings.width || sy >= mSettings.height) {
                continue;
            }
            Reservoir<DirectSample> &resCandidate = mDirectReservoirs.at(sx, sy);
            if(resCandidate.q == 0) {
                continue;
            }

            Math::Vector dirIn = resCandidate.sample.point - pntOffset;
            float d = dirIn.magnitude();
            dirIn = dirIn / d;
            float dot = dirIn * nrmFacing;
            float dot2 = std::abs(dirIn * resCandidate.sample.normal);
            
            float q = 0;
            if(dot > 0) {
                Math::Radiance irad = resCandidate.sample.radiance * dot2 * dot / (d * d);
                Math::Radiance rad = irad * surface.reflected(primaryHit.isect, dirIn);
                q = rad.magnitude();
            }

            res.addReservoir(resCandidate, q, 1.0f, sampler);
        }            

        if(res.W > 0) {
            Math::Vector dirIn = res.sample.point - pntOffset;
            float d = dirIn.magnitude();
            dirIn = dirIn / d;
            float dot = dirIn * nrmFacing;
            float dot2 = std::abs(dirIn * res.sample.normal);

            if(dot > 0) {
                Math::Ray ray(pntOffset, dirIn);
                Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                Object::Intersection isect2 = mScene.intersect(beam, FLT_MAX, true);

                if (isect2.valid() && &(isect2.primitive()) == res.sample.primitive) {
                    Math::Radiance irad = res.sample.radiance * dot2 * dot / (d * d);
                    Math::Radiance rad = irad * surface.reflected(primaryHit.isect, dirIn);
                    radDirect = rad * res.W;
                }
            }
        }

        addRadiance(x, y, sample, radDirect);
    }

    void RendererReSTIR::indirectIlluminatePixel(int x, int y, int sample, Math::Sampler &sampler, Reservoir<IndirectSample> indirectSamples[])
    {
        Math::Radiance radIndirect;
        PrimaryHit &primaryHit = mPrimaryHits.at(x, y);
        const Math::Normal &nrmFacing = primaryHit.isect.facingNormal(); 
        const Object::Surface &surface = primaryHit.isect.primitive().surface();

        const int N = mSettings.indirectSamples;
        for(int i=0; i<N; i++) {
            indirectSamples[i].clear();
        }

        const int R = mSettings.radius;
        for(int i=0; i<mSettings.candidates; i++) {
            Math::Point2D s = sampler.getValue2D();
            s = s * R * 2 + Math::Point2D(-R, -R);
            int sx = (int)std::floor(s.u() + x);
            int sy = (int)std::floor(s.v() + y);
            if(sx < 0 || sy < 0 || sx >= mSettings.width || sy >= mSettings.height) {
                continue;
            }
            Reservoir<IndirectSample> &resCandidate = mIndirectReservoirs.at(sx, sy);
            if(resCandidate.q == 0) {
                continue;
            }

            for(int i=0; i<N; i++) {
                Math::Vector r = resCandidate.sample.point - primaryHit.isect.point();
                Math::Vector q = resCandidate.sample.point - mPrimaryHits.at(sx, sy).isect.point();
                Math::Normal &n = resCandidate.sample.normal;
                float J = std::fabs((n * r) * q.magnitude2() / ((n * q) * r.magnitude2()));
                indirectSamples[i].addReservoir(resCandidate, resCandidate.q, J, sampler);
            }
        }
        
        for(int i=0; i<N; i++) {
            if(indirectSamples[i].W > 0) {
                float q = indirectSamples[i].sample.indirectRadiance.magnitude();
                Math::Vector dirIn = indirectSamples[i].sample.point - primaryHit.isect.point();
                float d = dirIn.magnitude();
                dirIn = dirIn / d;
                float dot = dirIn * nrmFacing;
                
                if(dot > 0) {
                    Math::Color reflected = surface.reflected(primaryHit.isect, dirIn);
                    radIndirect += indirectSamples[i].sample.indirectRadiance * dot * reflected * indirectSamples[i].W;
                }
            }
        }

        addRadiance(x, y, sample, radIndirect / N);
    }

    void RendererReSTIR::addRadiance(int x, int y, int sample, const Math::Radiance &radiance)
    {
        Math::Radiance radTotal = mTotalRadiance.get(x, y) + radiance;
        mTotalRadiance.set(x, y, radTotal);
        Math::Color color = Framebuffer::toneMap(radTotal / static_cast<float>(sample + 1));
        mRenderFramebuffer->setPixel(x, y, color);
    }
}