#include "Object/Brdf/Base.hpp"

#include <vector>

namespace Object {
	namespace Brdf {
        Base::~Base()
        {
        }

        Object::Radiance Base::reflected(const Object::Radiance &, const Math::Vector &, const Math::Normal &, const Math::Vector &, const Object::Color &) const
		{
			return Object::Radiance(0, 0, 0);
		}

        Object::Radiance Base::transmitted(const Object::Radiance &, const Math::Vector &, const Math::Normal &, const Object::Color &) const
		{
			return Object::Radiance(0, 0, 0);
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
