#ifndef OBJECT_ALBEDO_BASE_HPP
#define OBJECT_ALBEDO_BASE_HPP

#include "Math/Forwards.hpp"
#include "Object/Color.hpp"
#include "Parse/Forwards.hpp"

namespace Object {
namespace Albedo {

class Base
{
public:
	Base();
	virtual ~Base();

	static const Base *fromAst(AST *ast);

	virtual Object::Color color(const Math::Point &point) const = 0;
};

}
}

#endif