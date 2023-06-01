#ifndef RENDER_RENDERER_HPP
#define RENDER_RENDERER_HPP

#include "Render/Executor.hpp"
#include "Render/Framebuffer.hpp"
#include "Render/Lighter/Base.hpp"
#include "Render/Raster.hpp"

#include "Object/Scene.hpp"

#include <memory>

namespace Render {
    class Renderer {
    public:
        struct Settings
        {
            unsigned int width;
            unsigned int height;
            unsigned int minSamples;
            unsigned int maxSamples;
            float sampleThreshold;
        };
        Renderer(const Object::Scene &scene, const Settings &settings, std::unique_ptr<Lighter::Base> lighter);

        Render::Executor &executor();
        Render::Framebuffer &renderFramebuffer();
        Render::Framebuffer &sampleStatusFramebuffer();

        static Object::Color Renderer::toneMap(const Object::Radiance &rad);
        Object::Radiance sampleIrradiance(const Object::Intersection &intersection, Math::Sampler::Base &sampler, Math::Vector &incidentDirection) const;

    private:
        void renderPixel(int x, int y, int sample, Math::Sampler::Base &sampler);

        const Object::Scene &mScene;
        Settings mSettings;

        Render::Executor mExecutor;
        std::unique_ptr<Render::Framebuffer> mRenderFramebuffer;
        std::unique_ptr<Render::Framebuffer> mSampleStatusFramebuffer;
        std::unique_ptr<Render::Lighter::Base> mLighter;

        Render::Raster<Object::Radiance> mTotalRadiance;
    };
}
#endif