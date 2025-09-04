#ifndef MATH_IMPL_SAMPLER_HALTON_HPP
#define MATH_IMPL_SAMPLER_HALTON_HPP

#include "Math/Sampler.hpp"

#include <stdint.h>

#include "CLProxies.hpp"
#include "OpenCL.hpp"

namespace Math::Impl::Sampler {
    class Halton : public Math::Sampler
    {
    public:
        Halton(int width, int height);

        void startSample(unsigned int index = 0) override;
        void startSample(unsigned int x, unsigned int y, unsigned int sample) override;

        float getValue() override;

        void writeProxy(SamplerProxy &proxy, OpenCL::Allocator &clAllocator) const;

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
    };
}
#endif