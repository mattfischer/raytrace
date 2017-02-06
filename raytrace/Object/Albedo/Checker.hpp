#ifndef OBJECT_ALBEDO_CHECKER_HPP
#define OBJECT_ALBEDO_CHECKER_HPP

#include "Object/Albedo/Base.hpp"

namespace Object {
namespace Albedo {

class Checker : public Base
{
public:
	Checker(const Object::Color &color1, const Object::Color &color2);
	virtual ~Checker();

	virtual Object::Color color(const Math::Point &point) const;

	static Checker *fromAst(AST *ast);

protected:
	Object::Color mColor1;
	Object::Color mColor2;
};

}
}
#endif