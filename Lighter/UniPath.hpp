#ifndef LIGHTER_UNIPATH_HPP
#define LIGHTER_UNIPATH_HPP

#include "Lighter/Base.hpp"
#include "Object/PointLight.hpp"

namespace Lighter
{
    class UniPath : public Base
    {
    public:
        virtual Object::Radiance light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;

    private:
        Object::Radiance lightRadiant(const Object::Intersection &intersection) const;
        Object::Radiance lightTransmitted(const Object::Intersection &intersection, Render::Sampler &sampler) const;
        Object::Radiance lightReflected(const Object::Intersection &intersection, Render::Sampler &sampler) const;

        Object::Radiance sampleLight(const Object::Intersection &intersection, const Object::Primitive &light, Render::Sampler &sampler, Math::Vector &incidentDirection, float &pdfAngular) const;
        Object::Radiance evaluatePointLight(const Object::Intersection &intersection, const Object::PointLight &pointLight, Math::Vector &incidentDirection) const;
        Object::Radiance sampleBrdf(const Object::Intersection &intersection, const Object::Brdf::Base &brdf, Render::Sampler &sampler, Math::Vector &incidentDirection, float &lightPdf) const;
    };
}
#endif // UNIPATH_HPP
