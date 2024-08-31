#ifndef OBJECT_ALBEDO_BASE_HPP
#define OBJECT_ALBEDO_BASE_HPP

#include "Math/Color.hpp"

#include "Math/Point2D.hpp"
#include "Math/Bivector2D.hpp"

#include "Object/CLProxies.hpp"
#include "OpenCL.hpp"

namespace Object::Albedo {
    class Base
    {
    public:
        virtual Math::Color color(const Math::Point2D &surfacePoint, const Math::Bivector2D &surfaceProjection) const = 0;
        virtual bool needSurfaceProjection() const = 0;

        virtual void writeProxy(AlbedoProxy &proxy, OpenCL::Allocator &clAllocator) const = 0;
    };
}
#endif