#ifndef RENDER_LIGHT_PROBE_HPP
#define RENDER_LIGHT_PROBE_HPP

#include "Object/Intersection.hpp"
#include "Math/OrthonormalBasis.hpp"
#include "Math/Impl/Sampler/Random.hpp"

#include "Render/Cpu/Impl/Lighter/UniPath.hpp"

namespace Render {
    class LightProbe {
    public:
        LightProbe(const Object::Intersection &isect);
    
        void getSample(float &azimuth, float &elevation, Math::Color &color);

    private:
        const Object::Intersection &mIntersection;
        Math::OrthonormalBasis mBasis;
        Math::Impl::Sampler::Random mSampler;
        Render::Cpu::Impl::Lighter::UniPath mLighter;
    };
}

#endif