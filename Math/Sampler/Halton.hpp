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
            Halton(int width, int height);

            void startSample(unsigned int index = 0) override;
            void startSample(unsigned int x, unsigned int y, unsigned int sample) override;

            float getValue() override;

        private:
            uint64_t mIndex;
            unsigned int mNextDimension;

            unsigned int mWidthExponent;
            unsigned int mWidthAligned;
            unsigned int mHeightExponent;
            unsigned int mHeightAligned;
            unsigned int mSampleStride;

            int mEuclidX;
            int mEuclidY;
            
            std::default_random_engine mRandomEngine;
        };
    }
}
#endif