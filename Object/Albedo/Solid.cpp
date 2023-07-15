#include "Object/Albedo/Solid.hpp"

namespace Object {
    namespace Albedo {
        Solid::Solid(const Math::Color &color)
        : mColor(color)
        {
        }

        Math::Color Solid::color(const Math::Point2D &, const Math::Bivector2D &) const
        {
            return mColor;
        }

        bool Solid::needSurfaceProjection() const
        {
            return false;
        } 

        void Solid::writeProxy(AlbedoProxy &proxy, OpenCL::Allocator &clAllocator) const
        {
            proxy.type = AlbedoProxy::Type::Solid;
            mColor.writeProxy(proxy.solid.color);
        }       
    }
}
