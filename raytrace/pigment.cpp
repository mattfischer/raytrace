#include "pigment.h"

#include <math.h>

Pigment::Pigment()
{
}

Pigment::~Pigment()
{
}

PigmentSolid::PigmentSolid(const Color &color)
: mColor(color)
{
}

PigmentSolid::~PigmentSolid()
{
}

Color PigmentSolid::pointColor(const Vector &point) const
{
	return mColor;
}

PigmentChecker::PigmentChecker(const Color &color1, const Color &color2)
: mColor1(color1), mColor2(color2)
{
}

PigmentChecker::~PigmentChecker()
{
}

Color PigmentChecker::pointColor(const Vector &point) const
{
	int x = (int)abs(point.x()) + ((point.x() > 0)?1:0);
	int y = (int)abs(point.y()) + ((point.y() > 0)?1:0);
	int z = (int)abs(point.z()) + ((point.z() > 0)?1:0);

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