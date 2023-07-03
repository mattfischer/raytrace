#define _USE_MATH_DEFINES
#include "Object/Brdf/TorranceSparrow.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"
#include "Math/OrthonormalBasis.hpp"

#include <cmath>
#include <algorithm>

namespace Object {
    namespace Brdf {
        TorranceSparrow::TorranceSparrow(float strength, float roughness, float ior)
        {
            mStrength = strength;
            mRoughness = roughness;
            mIor = ior;
        }

        Math::Color TorranceSparrow::reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Math::Color &albedo) const
        {
            Math::Vector dirHalf = (dirIn + dirOut).normalize();
            float alpha = std::acos(std::min(1.0f, nrm * dirHalf));
            float cosAlpha = std::cos(alpha);
            float tanAlpha = std::tan(alpha);
            float m2 = mRoughness * mRoughness;

            float D = std::exp(-tanAlpha * tanAlpha / m2) / (M_PI * m2 * cosAlpha * cosAlpha * cosAlpha * cosAlpha);
            
            float cosThetaI = dirIn * nrm;
            float oneMinusCos = 1 - cosThetaI;
            float R0 = (1 - mIor) / (1 + mIor);
            R0 = R0 * R0;

            float F = R0 + (1 - R0) * oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos;

            float HN = dirHalf * nrm;
            float VH = dirHalf * dirOut;
            float VN = nrm * dirOut;
            float LN = nrm * dirIn;
            float G = std::min(1.0f, std::min(2 * HN * VN / VH, 2 * HN * LN / VH));

            return Math::Color(1, 1, 1) * mStrength * D * F * G / (4 * VN * LN);
        }

        Math::Color TorranceSparrow::transmitted(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Color &) const
        {
            float cosThetaI = dirIn * nrm;
            float oneMinusCos = 1 - cosThetaI;
            float R0 = (1 - mIor) / (1 + mIor);
            R0 = R0 * R0;

            float F = R0 + (1 - R0) * oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos;
            return Math::Color(1, 1, 1) * (1.0f - mStrength * F);
        }

        Math::Vector TorranceSparrow::sample(Math::Sampler::Base &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const
        {
            Math::Point2D samplePoint = sampler.getValue2D();
            float phi = 2 * M_PI * samplePoint.u();
            float tanTheta = std::sqrt(-mRoughness * mRoughness * std::log(1 - samplePoint.v()));
            float theta = std::atan(tanTheta);

            Math::OrthonormalBasis basis(nrm);

            Math::Vector axis = basis.localToWorld(Math::Vector::fromPolar(phi, M_PI / 2 - theta, 1));
            Math::Vector dirIn = -(dirOut - axis * (dirOut * axis * 2));

            return dirIn;
        }

        float TorranceSparrow::pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const
        {
            Math::Vector axis = (dirIn + dirOut).normalize();

            float cosTheta = axis * Math::Vector(nrm);
            float sinTheta = std::sqrt(std::max(0.0f, 1 - cosTheta * cosTheta));
            float tanTheta = sinTheta / cosTheta;

            float m2 = mRoughness * mRoughness;
            float pdf = std::exp(-tanTheta * tanTheta / m2) / (M_PI * m2 * cosTheta * cosTheta * cosTheta * cosTheta);
            pdf = pdf / (4 * (dirOut * axis));
            return std::min(pdf, 1000.0f);
        }

        bool TorranceSparrow::opaque() const
        {
            return false;
        }
    }
}
