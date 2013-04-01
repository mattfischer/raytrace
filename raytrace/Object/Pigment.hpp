#ifndef PIGMENT_H
#define PIGMENT_H

#include "Object/Color.hpp"
#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Parse/AST.h"

namespace Object {

class Pigment
{
public:
	Pigment();
	virtual ~Pigment();

	static Pigment *fromAst(AST *ast);

	virtual Color pointColor(const Math::Point &point) const = 0;
};

class PigmentSolid : public Pigment
{
public:
	PigmentSolid(const Color &color);
	virtual ~PigmentSolid();

	virtual Color pointColor(const Math::Point &point) const;

protected:
	Color mColor;
};

class PigmentChecker : public Pigment
{
public:
	PigmentChecker(const Color &color1, const Color &color2);
	virtual ~PigmentChecker();

	virtual Color pointColor(const Math::Point &point) const;

protected:
	Color mColor1;
	Color mColor2;
};

}

#endif