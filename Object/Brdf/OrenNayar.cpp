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

        Object::Color OrenNayar::reflected(const Math::Vector &dirIn, const Math::Vector &dirOut, const Object::Color &albedo) const
        {
            float cosThetaI = dirIn.z();
            float sinThetaI = std::sqrt(std::max(0.0f, 1 - cosThetaI * cosThetaI));
            float tanThetaI = sinThetaI / cosThetaI;

            float cosThetaR = dirOut.z();
            float sinThetaR = std::sqrt(std::max(0.0f, 1 - cosThetaR * cosThetaR));
            float tanThetaR = sinThetaR / cosThetaR;

            float cosPhi;
            if (sinThetaI < 0.001 || sinThetaR < 0.001) {
                cosPhi = 1;
            } else {
                Math::Vector projectedIncident = (dirIn - Math::Vector(0, 0, dirIn.z())) / sinThetaI;
                Math::Vector projectedOutgoing = (dirOut - Math::Vector(0, 0, dirOut.z())) / sinThetaR;
                cosPhi = projectedIncident * projectedOutgoing;
            }

            float sigma2 = mRoughness * mRoughness;

            float A = 1.0f - 0.5f * sigma2 / (sigma2 + 0.33f);
            float B = 0.45f * sigma2 / (sigma2 + 0.09f);

            float sinAlpha = std::max(sinThetaI, sinThetaR);
            float tanBeta = std::min(tanThetaI, tanThetaR);

            return albedo * mStrength * (A + B * std::max(0.0f, cosPhi) * sinAlpha * tanBeta) / (float)M_PI;
        }

        Math::Vector OrenNayar::sample(Math::Sampler::Base &sampler, const Math::Vector &) const
        {
            Math::Point2D samplePoint = sampler.getValue2D();
            float phi = 2 * M_PI * samplePoint.u();
            float theta = std::asin(std::sqrt(samplePoint.v()));

            Math::Vector dirIn = Math::Vector::fromPolar(phi, M_PI / 2 - theta, 1);

            return dirIn;
        }

        float OrenNayar::pdf(const Math::Vector &dirIn, const Math::Vector &) const
        {
            float cosTheta = std::max(dirIn.z(), 0.0f);
            return cosTheta / M_PI;
        }
    }
}
