#include "Object/Brdf/Base.hpp"

#include <vector>

namespace Object {
	namespace Brdf {
        Object::Radiance Base::reflected(const Object::Radiance &irradiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
		{
			return Object::Radiance(0, 0, 0);
		}

        Object::Radiance Base::transmitted(const Object::Radiance &irradiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const
		{
			return Object::Radiance(0, 0, 0);
		}

		float Base::lambert() const
		{
			return 0;
		}

		Math::Vector Base::sample(const Math::Point2D &samplePoint, const Math::Normal &normal, const Math::Vector &outgoingDirection) const
		{
			return Math::Vector(0, 0, 0);
		}

		float Base::pdf(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection) const
		{
			return 0;
		}
	}
}
