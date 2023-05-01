#include "Object/Surface.hpp"

#include "Object/Albedo/Base.hpp"
#include "Object/Brdf/Base.hpp"
#include "Object/Intersection.hpp"

#include "Parse/AST.h"

namespace Object {
    Surface::Surface(std::unique_ptr<Albedo::Base> albedo, std::unique_ptr<Brdf::Base> brdf, const Object::Radiance &radiance, std::unique_ptr<Object::NormalMap> normalMap)
    {
        mAlbedo = std::move(albedo);
        mBrdf = std::move(brdf);
        mRadiance = radiance;
        mNormalMap = std::move(normalMap);
    }

    const Albedo::Base &Surface::albedo() const
    {
        return *mAlbedo;
    }

    const Brdf::Base &Surface::brdf() const
    {
        return *mBrdf;
    }

    const Object::Radiance &Surface::radiance() const
    {
        return mRadiance;
    }

    bool Surface::hasNormalMap() const
    {
        return mNormalMap.get();
    }

    const Object::NormalMap &Surface::normalMap() const
    {
        return *mNormalMap;
    }

    Object::Color Surface::reflected(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const
    {
        return mBrdf->reflected(incidentDirection, intersection.facingNormal(), -intersection.ray().direction(), intersection.albedo());
    }

    Object::Color Surface::transmitted(const Object::Intersection &intersection, const Math::Vector &incidentDirection) const
    {
        return mBrdf->transmitted(incidentDirection, intersection.facingNormal(), intersection.albedo());
    }
}
