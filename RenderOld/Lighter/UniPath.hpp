#ifndef RENDER_LIGHTER_UNIPATH_HPP
#define RENDER_LIGHTER_UNIPATH_HPP

#include "Render/Lighter/Base.hpp"

namespace Render {
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
}

#endif
