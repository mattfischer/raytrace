#ifndef OBJECT_BRDF_OREN_NAYAR_HPP
#define OBJECT_BRDF_OREN_NAYAR_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
	namespace Brdf {
		class OrenNayar : public Base
		{
		public:
			OrenNayar(float strength, float roughness);

			virtual Object::Radiance radiance(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;
			virtual float lambert() const;

		private:
			float mStrength;
			float mRoughness;
		};
	}
}

#endif