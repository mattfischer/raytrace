#ifndef OBJECT_BRDF_TORRANCE_SPARROW_HPP
#define OBJECT_BRDF_TORRANCE_SPARROW_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
	namespace Brdf {
		class TorranceSparrow : public Base
		{
		public:
			TorranceSparrow(float strength, float roughness, float ior);

			virtual Object::Radiance reflected(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;
			virtual Object::Radiance transmitted(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const;

			virtual Math::Vector sample(float u, float v, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;
			float pdf(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;

		private:
			float mStrength;
			float mRoughness;
			float mIor;
		};
	}
}

#endif