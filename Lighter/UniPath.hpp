#ifndef LIGHTER_UNIPATH_HPP
#define LIGHTER_UNIPATH_HPP

#include "Lighter/Base.hpp"

namespace Lighter
{
    class UniPath : public Base
    {
    public:
        virtual Object::Radiance light(const Object::Intersection &isect, Render::Sampler &sampler) const;

    private:
        Object::Radiance lightInternal(const Object::Intersection &isect, Render::Sampler &sampler, int generation) const;
    };
}
#endif // UNIPATH_HPP
