#ifndef MATH_IMPL_SAMPLER_RANDOM_HPP
#define MATH_IMPL_SAMPLER_RANDOM_HPP

#include "Math/Sampler.hpp"

#include <vector>
#include <random>

namespace Math::Impl::Sampler {
    class Random : public Math::Sampler
    {
    public:
        void startSample(unsigned int index = 0) override;
        void startSample(unsigned int x, unsigned int y, unsigned int sample) override;

        float getValue() override;

    private:
        std::default_random_engine mRandomEngine;
    };
}
#endif