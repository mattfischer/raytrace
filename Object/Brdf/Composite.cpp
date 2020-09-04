#include "Object/Brdf/Composite.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"

#include "Object/Brdf/Lambert.hpp"
#include "Object/Brdf/Phong.hpp"

#include <vector>

namespace Object {
	namespace Brdf {
		Composite::Composite(std::unique_ptr<Base> diffuse, std::unique_ptr<Base> specular, float transmitIor)
		: mDiffuse(std::move(diffuse)), mSpecular(std::move(specular))
		{
			mTransmitIor = transmitIor;
		}

        Object::Radiance Composite::reflected(const Object::Radiance &irradiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
        {
            Object::Radiance radiance;
            Object::Radiance transmittedIrradiance = irradiance;

            if(hasSpecular()) {
                radiance += specular().reflected(transmittedIrradiance, incidentDirection, normal, outgoingDirection, albedo);
                transmittedIrradiance = specular().transmitted(transmittedIrradiance, incidentDirection, normal, albedo);
            }

            if(hasDiffuse()) {
                radiance += diffuse().reflected(transmittedIrradiance, incidentDirection, normal, outgoingDirection, albedo);
            }

            return radiance;
        }

        Object::Radiance Composite::transmitted(const Object::Radiance &irradiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const
        {
            Object::Radiance transmittedIrradiance = irradiance;

            if(hasSpecular()) {
                transmittedIrradiance = specular().transmitted(transmittedIrradiance, incidentDirection, normal, albedo);
            }

            if(hasDiffuse()) {
                transmittedIrradiance = diffuse().transmitted(transmittedIrradiance, incidentDirection, normal, albedo);
            }

            return transmittedIrradiance;
        }

		bool Composite::hasDiffuse() const
		{
			return mDiffuse.get();
		}

		const Base &Composite::diffuse() const
		{
			return *mDiffuse;
		}

		bool Composite::hasSpecular() const
		{
			return mSpecular.get();
		}

		const Base &Composite::specular() const
		{
			return *mSpecular;
		}

		bool Composite::hasTransmit() const
		{
			return mTransmitIor != 0;
		}

		float Composite::transmitIor() const
		{
			return mTransmitIor;
		}
	}
}
