#ifndef RENDER_LIGHT_PROBE_HPP
#define RENDER_LIGHT_PROBE_HPP

#include "Object/Intersection.hpp"
#include "Math/OrthonormalBasis.hpp"
#include "Math/Sampler/Random.hpp"

#include "Render/Cpu/Lighter/UniPath.hpp"

namespace Render {
    class LightProbe {
    public:
        LightProbe(const Object::Intersection &isect);
    
        void getSample(float &azimuth, float &elevation, Object::Color &color);

    private:
        const Object::Intersection &mIntersection;
        Math::OrthonormalBasis mBasis;
        Math::Sampler::Random mSampler;
        Render::Cpu::Lighter::UniPath mLighter;
    };
}

#endif