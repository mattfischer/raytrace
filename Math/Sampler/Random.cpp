#include "Math/Sampler/Random.hpp"

#include <algorithm>
#include <tuple>

namespace Math {
    namespace Sampler {
        void Random::startSequence(unsigned int index)
        {
        }

        void Random::startSequence(const Base::State &state)
        {
        }

        void Random::startSample()
        {
        }

        float Random::getValue()
        {
            std::uniform_real_distribution<float> dist(0, 1);
            return dist(mRandomEngine);
        }

        Math::Point2D Random::getValue2D()
        {
            return Math::Point2D(getValue(), getValue());
        }

        void Random::recordState(Base::State &state) const
        {
        }
    }
}