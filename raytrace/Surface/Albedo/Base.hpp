#ifndef SURFACE_ALBEDO_BASE_HPP
#define SURFACE_ALBEDO_BASE_HPP

#include "Object/Color.hpp"
#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Parse/AST.h"

namespace Surface {
namespace Albedo {

class Base
{
public:
	Base();
	virtual ~Base();

	static Base *fromAst(AST *ast);

	virtual Object::Color pointColor(const Math::Point &point) const = 0;
};

}
}

#endif