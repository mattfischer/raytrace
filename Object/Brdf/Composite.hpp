#ifndef OBJECT_BRDF_COMPOSITE_HPP
#define OBJECT_BRDF_COMPOSITE_HPP

#include "Object/Brdf/Base.hpp"

#include <vector>

namespace Object {
	namespace Brdf {
		class Composite
		{
		public:
            Composite(std::vector<std::unique_ptr<Base>> brdfs, float transmitIor);

            virtual Object::Radiance reflected(const Object::Radiance &irradiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;
            virtual Object::Radiance transmitted(const Object::Radiance &irradiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const;

            const std::vector<std::unique_ptr<Base>> &brdfs() const;
            float lambert() const;

			bool hasTransmit() const;
			float transmitIor() const;

		private:
            std::vector<std::unique_ptr<Base>> mBrdfs;
            float mTransmitIor;
            float mLambert;
		};
	}
}

#endif
