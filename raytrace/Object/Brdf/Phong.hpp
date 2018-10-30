#ifndef OBJECT_BRDF_PHONG_HPP
#define OBJECT_BRDF_PHONG_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
	namespace Brdf {
		class Phong : public Base
		{
		public:
			Phong(float strength, float power);

			virtual Object::Radiance reflected(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;
			virtual Object::Radiance transmitted(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const;

			virtual Math::Vector sample(const Math::Point2D &samplePoint, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;
			float pdf(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;

		private:
			float mStrength;
			float mPower;
		};
	}
}

#endif