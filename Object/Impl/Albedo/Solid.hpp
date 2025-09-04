#ifndef OBJECT_IMPL_ALBEDO_SOLID_HPP
#define OBJECT_IMPL_ALBEDO_SOLID_HPP

#include "Object/Albedo.hpp"

namespace Object::Impl::Albedo {
    class Solid : public Object::Albedo
    {
    public:
        Solid(const Math::Color &color);

        Math::Color color(const Math::Point2D &surfacePoint, const Math::Bivector2D &surfaceProjection) const override;
        bool needSurfaceProjection() const override;
    
        void writeProxy(AlbedoProxy &proxy, OpenCL::Allocator &clAllocator) const override;

    protected:
        Math::Color mColor;
    };
}
#endif