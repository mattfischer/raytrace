#ifndef OBJECT_POINT_LIGHT_HPP
#define OBJECT_POINT_LIGHT_HPP

#include "Math/Point.hpp"
#include "Math/Radiance.hpp"

#include "Proxies.hpp"

#include <memory>

namespace Object {
    class PointLight
    {
    public:
        PointLight(const Math::Point &position, const Math::Radiance &radiance);

        const Math::Point &position() const;
        const Math::Radiance &radiance() const;

        void writeProxy(PointLightProxy &proxy) const;

    private:
        Math::Point mPosition;
        Math::Radiance mRadiance;
    };
}

#endif