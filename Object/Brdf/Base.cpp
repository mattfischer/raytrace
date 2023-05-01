#include "Object/Brdf/Base.hpp"

#include <vector>

namespace Object {
    namespace Brdf {
        Base::~Base()
        {
        }

        Object::Color Base::reflected(const Math::Vector &, const Math::Normal &, const Math::Vector &, const Object::Color &) const
        {
            return Object::Color(0, 0, 0);
        }

        Object::Color Base::transmitted(const Math::Vector &, const Math::Normal &, const Object::Color &) const
        {
            return Object::Color(0, 0, 0);
        }

        float Base::lambert() const
        {
            return 0;
        }

        Math::Vector Base::sample(Render::Sampler &, const Math::Normal &, const Math::Vector &) const
        {
            return Math::Vector(0, 0, 0);
        }

        float Base::pdf(const Math::Vector &, const Math::Normal &, const Math::Vector &) const
        {
            return 0;
        }

        bool Base::opaque() const
        {
            return true;
        }

        float Base::transmitIor() const
        {
            return 1;
        }
    }
}
