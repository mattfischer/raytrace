#include "pigment.h"

#include <math.h>

#define EPSILON 0.01

Pigment::Pigment()
{
}

Pigment::~Pigment()
{
}

Pigment *Pigment::fromAst(AST *ast)
{
	AST *pigmentAst = ast->children[0];
	switch(pigmentAst->type)
	{
	case AstPigmentColor:
		return new PigmentSolid(pigmentAst->children[0]->data._vector);
	case AstPigmentChecker:
		return new PigmentChecker(pigmentAst->children[0]->data._vector, pigmentAst->children[1]->data._vector);
	}

	return 0;
}

PigmentSolid::PigmentSolid(const Color &color)
: mColor(color)
{
}

PigmentSolid::~PigmentSolid()
{
}

Color PigmentSolid::pointColor(const Math::Vector &point) const
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

Color PigmentChecker::pointColor(const Math::Vector &point) const
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