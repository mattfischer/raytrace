#define _USE_MATH_DEFINES
#include "Object/Brdf/OrenNayar.hpp"

#include "Math/OrthonormalBasis.hpp"

#include <cmath>
#include <algorithm>

namespace Object {
    namespace Brdf {
        OrenNayar::OrenNayar(float strength, float roughness)
        {
            mStrength = strength;
            mRoughness = roughness;
        }

        float OrenNayar::lambert() const
        {
            return mStrength;
        }

        Object::Color OrenNayar::reflected(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
        {
            float cosThetaI = incidentDirection * normal;
            float sinThetaI = std::sqrt(std::max(0.0f, 1 - cosThetaI * cosThetaI));
            float tanThetaI = sinThetaI / cosThetaI;

            float cosThetaR = outgoingDirection * normal;
            float sinThetaR = std::sqrt(std::max(0.0f, 1 - cosThetaR * cosThetaR));
            float tanThetaR = sinThetaR / cosThetaR;

            float cosPhi;
            if (sinThetaI < 0.001 || sinThetaR < 0.001) {
                cosPhi = 1;
            } else {
                Math::Vector projectedIncident = (incidentDirection - Math::Vector(normal) * (incidentDirection * normal)) / sinThetaI;
                Math::Vector projectedOutgoing = (outgoingDirection - Math::Vector(normal) * (outgoingDirection * normal)) / sinThetaR;
                cosPhi = projectedIncident * projectedOutgoing;
            }

            float sigma2 = mRoughness * mRoughness;

            float A = 1.0f - 0.5f * sigma2 / (sigma2 + 0.33f);
            float B = 0.45f * sigma2 / (sigma2 + 0.09f);

            float sinAlpha = std::max(sinThetaI, sinThetaR);
            float tanBeta = std::min(tanThetaI, tanThetaR);

            return albedo * mStrength * (A + B * std::max(0.0f, cosPhi) * sinAlpha * tanBeta) / M_PI;
        }

        Math::Vector OrenNayar::sample(Render::Sampler &sampler, const Math::Normal &normal, const Math::Vector &) const
        {
            Math::Point2D samplePoint = sampler.getValue2D();
            Math::OrthonormalBasis basis(normal);
            float phi = 2 * M_PI * samplePoint.u();
            float theta = std::asin(std::sqrt(samplePoint.v()));

            Math::Vector incidentDirection = basis.localToWorld(Math::Vector::fromPolar(phi, M_PI / 2 - theta, 1));

            return incidentDirection;
        }

        float OrenNayar::pdf(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &) const
        {
            float cosTheta = std::max(incidentDirection * Math::Vector(normal), 0.0f);
            return cosTheta / M_PI;
        }
    }
}
