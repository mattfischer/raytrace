#ifndef OBJECT_ALBEDO_SOLID_HPP
#define OBJECT_ALBEDO_SOLID_HPP

#include "Object/Albedo/Base.hpp"

namespace Object {
namespace Albedo {

class Solid : public Base
{
public:
	Solid(const Object::Color &color);
	virtual ~Solid();

	virtual Object::Color color(const Math::Point &point) const;

	static std::unique_ptr<Solid> fromAst(AST *ast);

protected:
	Object::Color mColor;
};

}
}
#endif