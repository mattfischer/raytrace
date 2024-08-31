#ifndef OBJECT_ALBEDO_SOLID_HPP
#define OBJECT_ALBEDO_SOLID_HPP

#include "Object/Albedo/Base.hpp"

namespace Object::Albedo {
    class Solid : public Base
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