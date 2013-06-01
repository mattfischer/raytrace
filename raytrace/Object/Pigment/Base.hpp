#ifndef OBJECT_PIGMENT_BASE_HPP
#define OBJECT_PIGMENT_BASE_HPP

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