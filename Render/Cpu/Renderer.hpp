#ifndef RENDER_CPU_RENDERER_HPP
#define RENDER_CPU_RENDERER_HPP

#include "Render/Renderer.hpp"

#include "Render/Executor.hpp"
#include "Render/Framebuffer.hpp"
#include "Render/Raster.hpp"

#include "Render/Cpu/Lighter/Base.hpp"

#include "Object/Scene.hpp"

#include <memory>

namespace Render {
    namespace Cpu {
        class Renderer : public Render::Renderer {
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

        private:
            void renderPixel(int x, int y, int sample, Math::Sampler::Base &sampler);

            const Object::Scene &mScene;
            Settings mSettings;

            std::unique_ptr<Render::Cpu::Lighter::Base> mLighter;

            Render::Raster<Object::Radiance> mTotalRadiance;
        };
    }
}
#endif