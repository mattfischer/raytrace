#ifndef MATH_SAMPLER_BASE_HPP
#define MATH_SAMPLER_BASE_HPP

#include "Math/Point2D.hpp"

namespace Math {
    namespace Sampler {
        class Base {
        public:
            virtual void startSample(unsigned int index = 0) = 0;
            virtual void startSample(unsigned int x, unsigned int y, unsigned int sample) = 0;

            virtual float getValue() = 0;
            Math::Point2D getValue2D()
            {
                return Math::Point2D(getValue(), getValue());
            }
        };
    }
}
#endif