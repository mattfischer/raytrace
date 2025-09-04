#ifndef OBJECT_SURFACE_HPP
#define OBJECT_SURFACE_HPP

#include "Object/Albedo.hpp"
#include "Object/Brdf.hpp"

#include "Math/Color.hpp"
#include "Math/Radiance.hpp"
#include "Object/NormalMap.hpp"

#include "Object/CLProxies.hpp"
#include "OpenCL.hpp"

#include <memory>

namespace Object {
    class Intersection;
    class Surface
    {
    public:
        Surface(std::unique_ptr<Object::Albedo> albedo, std::vector<std::unique_ptr<Object::Brdf>> brdfs, float transmitIor, const Math::Radiance &radiance, std::unique_ptr<Object::NormalMap> normalMap);

        const Object::Albedo &albedo() const;
        const Math::Radiance &radiance() const;
        bool hasNormalMap() const;
        const Object::NormalMap &normalMap() const;

        Math::Color reflected(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const;
        Math::Color transmitted(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const;
        Math::Color sample(const Object::Intersection &isect, Math::Sampler &sampler, Math::Vector &dirIn, float &pdf, bool &pdfDelta) const;
        float pdf(const Object::Intersection &isect, const Math::Vector &dirIn) const;

        bool opaque() const;
        float lambert() const;
        float transmitIor() const;

        void writeProxy(SurfaceProxy &proxy, OpenCL::Allocator &clAllocator) const;

    private:
        std::unique_ptr<Object::Albedo> mAlbedo;
        std::vector<std::unique_ptr<Object::Brdf>> mBrdfs;
        float mTransmitIor;
        Math::Radiance mRadiance;
        std::unique_ptr<Object::NormalMap> mNormalMap;

        float mLambert;
        bool mOpaque;
    };
}

#endif
