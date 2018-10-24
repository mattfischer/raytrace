#define _USE_MATH_DEFINES
#include "Object/Brdf/OrenNayar.hpp"

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

		Object::Radiance OrenNayar::reflected(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
		{
			float thetaI = std::acos(std::min(1.0f, incidentDirection * normal));
			float sinThetaI = std::sin(thetaI);
			
			float thetaR = std::acos(std::min(1.0f, outgoingDirection * normal));
			float sinThetaR = std::sin(thetaR);

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

			float alpha = std::max(thetaI, thetaR);
			float beta = std::min(thetaI, thetaR);

			return incidentRadiance * albedo * mStrength * (A + B * std::max(0.0f, cosPhi) * std::sin(alpha) * std::tan(beta)) / M_PI;
		}
	}
}