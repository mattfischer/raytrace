#ifndef LIGHTER_UNIPATH_HPP
#define LIGHTER_UNIPATH_HPP

#include "Lighter/Base.hpp"

namespace Lighter
{
    class UniPath : public Base
    {
    public:
        Object::Radiance light(const Object::Intersection &isect, Math::Sampler::Base &sampler) const override;

    private:
        Object::Radiance lightInternal(const Object::Intersection &isect, Math::Sampler::Base &sampler, int generation) const;
    };
}
#endif // UNIPATH_HPP
