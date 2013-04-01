#include "Object/Pigment/Checker.hpp"

#include <math.h>

#define EPSILON 0.01

namespace Object {
namespace Pigment {

Checker::Checker(const Color &color1, const Color &color2)
: mColor1(color1), mColor2(color2)
{
}

Checker::~Checker()
{
}

Color Checker::pointColor(const Math::Point &point) const
{
	int x = (int)abs(point.x() + EPSILON) + ((point.x() + EPSILON > 0)?1:0);
	int y = (int)abs(point.y() + EPSILON) + ((point.y() + EPSILON > 0)?1:0);
	int z = (int)abs(point.z() + EPSILON) + ((point.z() + EPSILON > 0)?1:0);

	int checker = (x % 2 + y % 2 + z % 2) % 2;

	if(checker==0)
	{
		return mColor1;
	}
	else
	{
		return mColor2;
	}
}

}
}