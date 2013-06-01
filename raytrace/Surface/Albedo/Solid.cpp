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

Object::Color Solid::pointColor(const Math::Point &point) const
{
	return mColor;
}

}
}