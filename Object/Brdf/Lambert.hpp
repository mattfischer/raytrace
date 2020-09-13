#ifndef OBJECT_BRDF_LAMBERT_HPP
#define OBJECT_BRDF_LAMBERT_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
	namespace Brdf {
		class Lambert : public Base
		{
		public:
			Lambert(float strength);

            virtual Object::Radiance reflected(const Object::Radiance &irradiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;
			virtual float lambert() const;

            Math::Vector sample(Render::Sampler &sampler, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;
            float pdf(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection) const;

		private:
			float mStrength;
		};
	}
}

#endif
