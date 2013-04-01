#ifndef PIGMENT_H
#define PIGMENT_H

#include "Object/Color.hpp"
#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Parse/AST.h"

namespace Object {
namespace Pigment {

class Base
{
public:
	Base();
	virtual ~Base();

	static Base *fromAst(AST *ast);

	virtual Color pointColor(const Math::Point &point) const = 0;
};

}
}

#endif