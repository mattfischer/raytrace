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
            mOpaque = false;
            for(const std::unique_ptr<Base> &brdf : mBrdfs) {
                if(brdf->opaque()) {
                    mOpaque = true;
                }

                if(brdf->lambert() > 0) {
                    mLambert = brdf->lambert();
                    break;
                }
            }

            mTransmitIor = transmitIor;
        }

        Object::Color Composite::reflected(const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
        {
            Object::Color color;
            Object::Color transmittedColor(1, 1, 1);

            for(const std::unique_ptr<Base> &brdf : mBrdfs) {
                color = color + transmittedColor * brdf->reflected(incidentDirection, normal, outgoingDirection, albedo);
                transmittedColor = transmittedColor * brdf->transmitted(incidentDirection, normal, albedo);
            }

            return color;
        }

        Object::Color Composite::transmitted(const Math::Vector &incidentDirection, const Math::Normal &normal, const Object::Color &albedo) const
        {
            Object::Color transmittedColor(1, 1, 1);

            for(const std::unique_ptr<Base> &brdf : mBrdfs) {
                transmittedColor = transmittedColor * brdf->transmitted(incidentDirection, normal, albedo);
            }

            return transmittedColor;
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

        float Composite::lambert() const
        {
            return mLambert;
        }

        bool Composite::opaque() const
        {
            return mOpaque;
        }

        float Composite::transmitIor() const
        {
            return mTransmitIor;
        }
    }
}
