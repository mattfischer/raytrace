#include "Math/Impl/Sampler/Random.hpp"

#include <algorithm>
#include <tuple>

namespace Math::Impl::Sampler {
    void Random::startSample(unsigned int index)
    {
    }

    void Random::startSample(unsigned int x, unsigned int y, unsigned int sample)
    {
    }

    float Random::getValue()
    {
        std::uniform_real_distribution<float> dist(0, 1);
        float value = dist(mRandomEngine);
        if(value == 1.0f) {
            value = 0.0f;
        }

        return value;
    }
}