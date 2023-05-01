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
        Surface(std::unique_ptr<Albedo::Base> albedo, std::unique_ptr<Brdf::Base> brdf, const Object::Radiance &radiance, std::unique_ptr<Object::NormalMap> normalMap);

        const Albedo::Base &albedo() const;
        const Brdf::Base &brdf() const;
        const Object::Radiance &radiance() const;
        bool hasNormalMap() const;
        const Object::NormalMap &normalMap() const;

        Object::Color reflected(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const;
        Object::Color transmitted(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const;

        Object::Color albedo(const Object::Intersection &intersection) const;
        const Math::Bivector2D &surfaceProjection(const Object::Intersection &intersection) const;
        const Math::Normal &normal(const Object::Intersection &intersection) const;
        const Math::Normal &facingNormal(const Object::Intersection &intersection) const;
        
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
        std::unique_ptr<Brdf::Base> mBrdf;
        Object::Radiance mRadiance;
        std::unique_ptr<Object::NormalMap> mNormalMap;
    };
}

#endif
