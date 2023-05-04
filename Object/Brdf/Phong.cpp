#define _USE_MATH_DEFINES
#include "Object/Brdf/Phong.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"
#include "Math/OrthonormalBasis.hpp"

#include <cmath>
#include <algorithm>

namespace Object {
    namespace Brdf {
        Phong::Phong(float strength, float power)
        {
            mStrength = strength;
            mPower = power;
        }

        Object::Color Phong::reflected(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &) const
        {
            Math::Vector reflectDirection = -(incidentDirection - Math::Vector(normal) * (2 * (normal * incidentDirection)));

            float dot = reflectDirection * outgoingDirection;
            float coeff = 0;
            if(dot > 0) {
                coeff = std::pow(dot, mPower);
            }

            return Color(1, 1, 1) * mStrength * coeff * (mPower + 1) / (2 * (float)M_PI);
        }

        Object::Color Phong::transmitted(const Math::Vector &, const Math::Normal &, const Object::Color &) const
        {
            return Color(1, 1, 1) * (1.0f - mStrength);
        }

        Math::Vector Phong::sample(Render::Sampler &sampler, const Math::Normal &normal, const Math::Vector &outgoingDirection) const
        {
            Math::Point2D samplePoint = sampler.getValue2D();
            float phi = 2 * M_PI * samplePoint.u();
            float theta = std::acos(std::pow(samplePoint.v(), 1.0f / (mPower + 1)));

            Math::OrthonormalBasis basis(outgoingDirection);

            Math::Vector reflectDirection = basis.localToWorld(Math::Vector::fromPolar(phi, M_PI / 2 - theta, 1));
            Math::Vector incidentDirection = -(reflectDirection - Math::Vector(normal) * (reflectDirection * normal * 2));

            return incidentDirection;
        }

        float Phong::pdf(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection) const
        {
            float coeff = 0;
            Math::Vector reflectDirection = -(incidentDirection - Math::Vector(normal) * (incidentDirection * normal * 2));
            float dot = reflectDirection * outgoingDirection;
            if (dot > 0) {
                coeff = std::pow(dot, mPower);
            }

            float pdf = coeff * (mPower + 1) / (2 * M_PI);
            return std::min(pdf, 1000.0f);
        }
    }
}
