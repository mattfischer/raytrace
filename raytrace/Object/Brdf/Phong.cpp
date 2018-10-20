#define _USE_MATH_DEFINES
#include "Object/Brdf/Phong.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"
#include "Math/OrthonormalBasis.hpp"

#include "Lighter/Utils.hpp"

#include <cmath>

namespace Object {
	namespace Brdf {
		Phong::Phong(float strength, float power)
		{
			mStrength = strength;
			mPower = power;
		}

		Object::Radiance Phong::radiance(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
		{
			Math::Vector reflectDirection = -(incidentDirection - Math::Vector(normal) * (2 * (normal * incidentDirection)));

			float dot = reflectDirection * outgoingDirection;
			float coeff = 0;
			if(dot > 0) {
				coeff = std::pow(dot, mPower);
			}

			return incidentRadiance * mStrength * coeff * (mPower + 1) / (2 * M_PI);
		}

		Math::Vector Phong::sample(float u, float v, const Math::Normal &normal, const Math::Vector &outgoingDirection, float &pdf) const
		{
			float phi = 2 * M_PI * u;
			float theta = std::acos(std::pow(v, 1.0f / (mPower + 1)));

			Math::OrthonormalBasis basis(outgoingDirection);

			Math::Vector direction = basis.localToWorld(Math::Vector::fromPolar(phi, M_PI / 2 - theta, 1));
			Math::Vector incidentDirection = -(direction - Math::Vector(normal) * (direction * normal * 2));

			float coeff = 0;
			float dot = direction * outgoingDirection;
			if (dot > 0) {
				coeff = std::pow(dot, mPower);
			}

			pdf = coeff * (mPower + 1) / (2 * M_PI);

			return incidentDirection;
		}
	}
}