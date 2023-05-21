#ifndef MATH_SAMPLER_RANDOM_HPP
#define MATH_SAMPLER_RANDOM_HPP

#include "Math/Sampler/Base.hpp"

#include <vector>
#include <random>

namespace Math {
    namespace Sampler {
        class Random : public Base
        {
        public:
            void startSequence(unsigned int index = 0) override;
            void startSequence(const Base::State &state) override;
            void startSample() override;

            float getValue() override;
            Math::Point2D getValue2D() override;

            void recordState(Base::State &state) const override;

        private:
            std::default_random_engine mRandomEngine;
        };
    }
}
#endif