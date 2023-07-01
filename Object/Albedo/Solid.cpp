#include "Object/Albedo/Solid.hpp"

namespace Object {
    namespace Albedo {
        Solid::Solid(const Object::Color &color)
        : mColor(color)
        {
        }

        Object::Color Solid::color(const Math::Point2D &, const Math::Bivector2D &) const
        {
            return mColor;
        }

        bool Solid::needSurfaceProjection() const
        {
            return false;
        } 

        void Solid::writeProxy(AlbedoProxy &proxy) const
        {
            proxy.type = AlbedoProxy::Type::Solid;
            mColor.writeProxy(proxy.solid.color);
        }       
    }
}
