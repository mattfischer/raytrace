#include "Surface/Albedo/Solid.hpp"

namespace Surface {
namespace Albedo {

Solid::Solid(const Object::Color &color)
: mColor(color)
{
}

Solid::~Solid()
{
}

Object::Color Solid::color(const Math::Point &point) const
{
	return mColor;
}

}
}