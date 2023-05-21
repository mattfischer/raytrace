#ifndef OBJECT_SURFACE_HPP
#define OBJECT_SURFACE_HPP

#include "Object/Albedo/Base.hpp"
#include "Object/Brdf/Base.hpp"

#include "Object/Color.hpp"
#include "Object/Radiance.hpp"
#include "Object/NormalMap.hpp"

#include <memory>

namespace Object {
    class Intersection;
    class Surface
    {
    public:
        Surface(std::unique_ptr<Albedo::Base> albedo, std::vector<std::unique_ptr<Brdf::Base>> brdfs, float transmitIor, const Object::Radiance &radiance, std::unique_ptr<Object::NormalMap> normalMap);

        const Albedo::Base &albedo() const;
        const Object::Radiance &radiance() const;
        bool hasNormalMap() const;
        const Object::NormalMap &normalMap() const;

        Object::Color reflected(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const;
        Object::Color transmitted(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const;

        Object::Color albedo(const Object::Intersection &isect) const;
        const Math::Bivector2D &surfaceProjection(const Object::Intersection &isect) const;
        const Math::Normal &normal(const Object::Intersection &isect) const;
        const Math::Normal &facingNormal(const Object::Intersection &isect) const;

        Object::Color sample(const Object::Intersection &isect, Math::Sampler::Base &sampler, Math::Vector &dirIn, float &pdf, bool &pdfDelta) const;

        float pdf(const Object::Intersection &isect, const Math::Vector &dirIn) const;
        bool opaque() const;
        float lambert() const;
        float transmitIor() const;

    private:
        friend class Intersection;
        struct IntersectionCache {
            bool surfaceProjectionValid;
            Math::Bivector2D surfaceProjection;

            bool albedoValid;
            Object::Color albedo;

            bool normalValid;
            Math::Normal normal;
            Math::Normal facingNormal;
        };
        static void initIntersectionCache(IntersectionCache &intersectionCache);

        std::unique_ptr<Albedo::Base> mAlbedo;
        std::vector<std::unique_ptr<Brdf::Base>> mBrdfs;
        float mTransmitIor;
        Object::Radiance mRadiance;
        std::unique_ptr<Object::NormalMap> mNormalMap;

        float mLambert;
        bool mOpaque;
    };
}

#endif
