#ifndef MATH_SAMPLER_BASE_HPP
#define MATH_SAMPLER_BASE_HPP

#include "Math/Point2D.hpp"

namespace Math {
    namespace Sampler {
        class Base {
        public:
            struct State {
            };

            virtual void startSequence(unsigned int index = 0) = 0;
            virtual void startSequence(const State &state) = 0;
            virtual void startSample() = 0;

            virtual float getValue() = 0;
            virtual Math::Point2D getValue2D() = 0;

            virtual void recordState(State &state) const = 0;
        };
    }
}
#endif