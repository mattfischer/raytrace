#define _USE_MATH_DEFINES
#include "Object/Brdf/Lambert.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"
#include "Math/OrthonormalBasis.hpp"

#include <cmath>

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

        Object::Radiance Lambert::reflected(const Object::Radiance &irradiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
		{
            return irradiance * albedo * mStrength / M_PI;
		}

        Math::Vector Lambert::sample(const Math::Point2D &samplePoint, const Math::Normal &normal, const Math::Vector &outgoingDirection) const
        {
            Math::OrthonormalBasis basis(normal);
            float phi = 2 * M_PI * samplePoint.u();
            float theta = std::asin(std::sqrt(samplePoint.v()));

            Math::Vector incidentDirection = basis.localToWorld(Math::Vector::fromPolar(phi, M_PI / 2 - theta, 1));

            return incidentDirection;
        }

        float Lambert::pdf(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection) const
        {
            float cosTheta = incidentDirection * Math::Vector(normal);
            return cosTheta / M_PI;
        }
	}
}
