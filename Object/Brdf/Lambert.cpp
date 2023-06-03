#define _USE_MATH_DEFINES
#include "Object/Brdf/Lambert.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"
#include "Math/OrthonormalBasis.hpp"

#include <cmath>
#include <algorithm>

namespace Object {
    namespace Brdf {
        Lambert::Lambert(float strength)
        {
            mStrength = strength;
        }

        float Lambert::lambert() const
        {
            return mStrength;
        }

        Object::Color Lambert::reflected(const Math::Vector &, const Math::Vector &, const Object::Color &albedo) const
        {
            return albedo * mStrength / (float)M_PI;
        }

        Math::Vector Lambert::sample(Math::Sampler::Base &sampler, const Math::Vector &) const
        {
            Math::Point2D samplePoint = sampler.getValue2D();
            float phi = 2 * M_PI * samplePoint.u();
            float theta = std::asin(std::sqrt(samplePoint.v()));

            Math::Vector dirIn = Math::Vector::fromPolar(phi, M_PI / 2 - theta, 1);

            return dirIn;
        }

        float Lambert::pdf(const Math::Vector &dirIn, const Math::Vector &) const
        {
            float cosTheta = std::max(dirIn.z(), 0.0f);
            float pdf = cosTheta / M_PI;

            return pdf;
        }
    }
}
