#include "Object/Albedo/Solid.hpp"

namespace Object {
	namespace Albedo {
		Solid::Solid(const Object::Color &color)
		: mColor(color)
		{
		}

		Object::Color Solid::color(const Math::Point &point) const
		{
			return mColor;
		}
	}
}