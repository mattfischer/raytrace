#define _USE_MATH_DEFINES
#include "Object/Brdf/Phong.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"
#include "Math/OrthonormalBasis.hpp"

#include <cmath>

namespace Object {
	namespace Brdf {
		Phong::Phong(float strength, float power)
		{
			mStrength = strength;
			mPower = power;
		}

		Object::Radiance Phong::reflected(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
		{
			Math::Vector reflectDirection = -(incidentDirection - Math::Vector(normal) * (2 * (normal * incidentDirection)));

			float dot = reflectDirection * outgoingDirection;
			float coeff = 0;
			if(dot > 0) {
				coeff = std::pow(dot, mPower);
			}

			return incidentRadiance * mStrength * coeff * (mPower + 1) / (2 * M_PI);
		}

		Object::Radiance Phong::transmitted(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const
		{
			return incidentRadiance * (1.0f - mStrength);
		}

		Math::Vector Phong::sample(float u, float v, const Math::Normal &normal, const Math::Vector &outgoingDirection) const
		{
			float phi = 2 * M_PI * u;
			float theta = std::acos(std::pow(v, 1.0f / (mPower + 1)));

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

			return coeff * (mPower + 1) / (2 * M_PI);
		}
	}
}