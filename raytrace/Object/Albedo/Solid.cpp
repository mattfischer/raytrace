#include "Object/Albedo/Solid.hpp"

#include "Parse/AST.h"

namespace Object {
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