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
            void startSample(unsigned int index = 0) override;
            void startSample(unsigned int x, unsigned int y, unsigned int sample) override;

            float getValue() override;

        private:
            std::default_random_engine mRandomEngine;
        };
    }
}
#endif