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

		Object::Radiance TorranceSparrow::reflected(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
		{
			Math::Vector half = (incidentDirection + outgoingDirection).normalize();
			float alpha = std::acos(std::min(1.0f, normal * half));
			float cosAlpha = std::cos(alpha);
			float tanAlpha = std::tan(alpha);
			float m2 = mRoughness * mRoughness;

			float D = std::exp(-tanAlpha * tanAlpha / m2) / (M_PI * m2 * cosAlpha * cosAlpha * cosAlpha * cosAlpha);
			
			float cosThetaI = incidentDirection * half;
			float oneMinusCos = 1 - cosThetaI;
			float R0 = (1 - mIor) / (1 + mIor);
			R0 = R0 * R0;

			float F = R0 + (1 - R0) * oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos;

			float HN = half * normal;
			float VH = half * outgoingDirection;
			float LH = half * incidentDirection;
			float VN = normal * outgoingDirection;
			float LN = normal * incidentDirection;
			float G = std::min(1.0f, std::min(2 * HN * VN / VH, 2 * HN * LN / VH));

			return incidentRadiance * mStrength * D * F * G / (4 * VN * LN);
		}

		Object::Radiance TorranceSparrow::transmitted(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const
		{
			float cosThetaI = incidentDirection * normal;
			float oneMinusCos = 1 - cosThetaI;
			float R0 = (1 - mIor) / (1 + mIor);
			R0 = R0 * R0;

			float F = R0 + (1 - R0) * oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos * oneMinusCos;
			return incidentRadiance * (1.0f - mStrength * F);
		}

		Math::Vector TorranceSparrow::sample(float u, float v, const Math::Normal &normal, const Math::Vector &outgoingDirection, float &pdf) const
		{
			float phi = 2 * M_PI * u;
			float tanTheta = std::sqrt(-mRoughness * mRoughness * std::log(1 - v));
			float theta = std::atan(tanTheta);

			Math::OrthonormalBasis basis(normal);

			Math::Vector direction = basis.localToWorld(Math::Vector::fromPolar(phi, M_PI / 2 - theta, 1));
			Math::Vector incidentDirection = -(outgoingDirection - direction * (outgoingDirection * direction * 2));

			float sinTheta = std::sin(theta);
			float cosTheta = std::cos(theta);
			float m2 = mRoughness * mRoughness;
			pdf = std::exp(-tanTheta * tanTheta / m2) / (M_PI * m2 * cosTheta * cosTheta * cosTheta * cosTheta);
			pdf = pdf / (4 * (outgoingDirection * direction));
			return incidentDirection;
		}
	}
}