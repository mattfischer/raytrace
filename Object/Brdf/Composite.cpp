#include "Object/Brdf/Composite.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"

#include "Object/Brdf/Lambert.hpp"
#include "Object/Brdf/Phong.hpp"

#include <vector>

namespace Object {
	namespace Brdf {
        Composite::Composite(std::vector<std::unique_ptr<Base>> brdfs, float transmitIor)
            : mBrdfs(std::move(brdfs))
        {
            mLambert = 0;
            for(const std::unique_ptr<Base> &brdf : mBrdfs) {
                if(brdf->lambert() > 0) {
                    mLambert = brdf->lambert();
                    break;
                }
            }

            mTransmitIor = transmitIor;
        }

        Object::Radiance Composite::reflected(const Object::Radiance &irradiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
        {
            Object::Radiance radiance;
            Object::Radiance transmittedIrradiance = irradiance;

            for(const std::unique_ptr<Base> &brdf : mBrdfs) {
                radiance += brdf->reflected(transmittedIrradiance, incidentDirection, normal, outgoingDirection, albedo);
                transmittedIrradiance = brdf->transmitted(transmittedIrradiance, incidentDirection, normal, albedo);
            }

            return radiance;
        }

        Object::Radiance Composite::transmitted(const Object::Radiance &irradiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const
        {
            Object::Radiance transmittedIrradiance = irradiance;

            for(const std::unique_ptr<Base> &brdf : mBrdfs) {
                transmittedIrradiance = brdf->transmitted(transmittedIrradiance, incidentDirection, normal, albedo);
            }

            return transmittedIrradiance;
        }

        Math::Vector Composite::sample(Render::Sampler &sampler, const Math::Normal &normal, const Math::Vector &outgoingDirection) const
        {
            float sample = sampler.getValue();
            int idx = std::min((int)std::floor(mBrdfs.size() * sample), (int)mBrdfs.size() - 1);
            const Object::Brdf::Base &brdf = *mBrdfs[idx];

            return brdf.sample(sampler, normal, outgoingDirection);
        }

        float Composite::pdf(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection) const
        {
            float totalPdf = 0;
            for(const std::unique_ptr<Object::Brdf::Base> &brdf : mBrdfs) {
                totalPdf += brdf->pdf(incidentDirection, normal, outgoingDirection);
            }
            totalPdf /= (float)mBrdfs.size();

            return totalPdf;
        }

        const std::vector<std::unique_ptr<Base>> &Composite::brdfs() const
        {
            return mBrdfs;
        }

        float Composite::lambert() const
        {
            return mLambert;
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
