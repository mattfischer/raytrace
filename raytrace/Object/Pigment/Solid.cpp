#include "Object/Pigment/Solid.hpp"

namespace Object {
namespace Pigment {

Solid::Solid(const Color &color)
: mColor(color)
{
}

Solid::~Solid()
{
}

Color Solid::pointColor(const Math::Point &point) const
{
	return mColor;
}

}
}