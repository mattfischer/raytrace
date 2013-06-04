#include "Surface/Albedo/Solid.hpp"

#include "Parse/AST.h"

namespace Surface {
namespace Albedo {

Solid::Solid(const Object::Color &color)
: mColor(color)
{
}

Solid::~Solid()
{
}

Object::Color Solid::color(const Math::Point &point) const
{
	return mColor;
}

Solid *Solid::fromAst(AST *ast)
{
	return new Solid(ast->children[0]->data._vector);
}

}
}