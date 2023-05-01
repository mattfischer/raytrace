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

        Object::Color Lambert::reflected(const Math::Vector &, const Math::Normal &, const Math::Vector &, const Object::Color &albedo) const
        {
            return albedo * mStrength / M_PI;
        }

        Math::Vector Lambert::sample(Render::Sampler &sampler, const Math::Normal &normal, const Math::Vector &) const
        {
            Math::Point2D samplePoint = sampler.getValue2D();
            Math::OrthonormalBasis basis(normal);
            float phi = 2 * M_PI * samplePoint.u();
            float theta = std::asin(std::sqrt(samplePoint.v()));

            Math::Vector incidentDirection = basis.localToWorld(Math::Vector::fromPolar(phi, M_PI / 2 - theta, 1));

            return incidentDirection;
        }

        float Lambert::pdf(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &) const
        {
            float cosTheta = std::max(incidentDirection * Math::Vector(normal), 0.0f);
            float pdf = cosTheta / M_PI;

            return pdf;
        }
    }
}
