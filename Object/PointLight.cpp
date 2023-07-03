#include "Object/PointLight.hpp"

#include "Math/Vector.hpp"
#include "Math/Transformation.hpp"

namespace Object {
    PointLight::PointLight(const Math::Point &position, const Math::Radiance &radiance)
        : mPosition(position), mRadiance(radiance)
    {
    }

    const Math::Point &PointLight::position() const
    {
        return mPosition;
    }

    const Math::Radiance &PointLight::radiance() const
    {
        return mRadiance;
    }

    void PointLight::writeProxy(PointLightProxy &proxy) const
    {
        mPosition.writeProxy(proxy.position);
        mRadiance.writeProxy(proxy.radiance);
    }
}