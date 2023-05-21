#ifndef MATH_SAMPLER_HALTON_HPP
#define MATH_SAMPLER_HALTON_HPP

#include "Math/Sampler/Base.hpp"

#include <vector>
#include <random>

namespace Math {
    namespace Sampler {
        class Halton : public Base
        {
        public:
            Halton(unsigned int numDimensions);

            void startSequence(unsigned int index = 0) override;
            void startSequence(const Base::State &state) override;
            void startSample() override;

            float getValue() override;
            Math::Point2D getValue2D() override;

            void recordState(Base::State &state) const override;

        private:
            struct State : public Base::State {
                std::vector<std::tuple<unsigned int, unsigned int>> lastSamples;
            };

            unsigned int mNumDimensions;
            unsigned int mCurrentDimension;
            std::default_random_engine mRandomEngine;

            std::vector<std::tuple<unsigned int, unsigned int>> mLastSamples;
        };
    }
}
#endif