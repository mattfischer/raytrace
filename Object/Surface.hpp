#ifndef OBJECT_SURFACE_HPP
#define OBJECT_SURFACE_HPP

#include "Object/Albedo/Base.hpp"
#include "Object/Brdf/Base.hpp"

#include "Math/Color.hpp"
#include "Math/Radiance.hpp"
#include "Object/NormalMap.hpp"

#include "Object/CLProxies.hpp"

#include <memory>

namespace Object {
    class Intersection;
    class Surface
    {
    public:
        Surface(std::unique_ptr<Albedo::Base> albedo, std::vector<std::unique_ptr<Brdf::Base>> brdfs, float transmitIor, const Math::Radiance &radiance, std::unique_ptr<Object::NormalMap> normalMap);

        const Albedo::Base &albedo() const;
        const Math::Radiance &radiance() const;
        bool hasNormalMap() const;
        const Object::NormalMap &normalMap() const;

        Math::Color reflected(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const;
        Math::Color transmitted(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const;
        Math::Color sample(const Object::Intersection &isect, Math::Sampler::Base &sampler, Math::Vector &dirIn, float &pdf, bool &pdfDelta) const;
        float pdf(const Object::Intersection &isect, const Math::Vector &dirIn) const;

        bool opaque() const;
        float lambert() const;
        float transmitIor() const;

        void writeProxy(SurfaceProxy &proxy) const;

    private:
        std::unique_ptr<Albedo::Base> mAlbedo;
        std::vector<std::unique_ptr<Brdf::Base>> mBrdfs;
        float mTransmitIor;
        Math::Radiance mRadiance;
        std::unique_ptr<Object::NormalMap> mNormalMap;

        float mLambert;
        bool mOpaque;
    };
}

#endif
