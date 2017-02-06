#include "Object/Brdf/Ambient.hpp"

#include "Parse/AST.h"

namespace Object {
namespace Brdf {

Ambient::Ambient(float strength)
{
	mStrength = strength;
}

Object::Color Ambient::color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
{
	return incidentColor * albedo * mStrength;
}

Ambient *Ambient::fromAst(AST *ast)
{
	return new Ambient(ast->data._float);
}

}
}