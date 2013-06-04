#ifndef SURFACE_ALBEDO_SOLID_HPP
#define SURFACE_ALBEDO_SOLID_HPP

#include "Surface/Albedo/Base.hpp"

namespace Surface {
namespace Albedo {

class Solid : public Base
{
public:
	Solid(const Object::Color &color);
	virtual ~Solid();

	virtual Object::Color color(const Math::Point &point) const;

	static Solid *fromAst(AST *ast);

protected:
	Object::Color mColor;
};

}
}
#endif