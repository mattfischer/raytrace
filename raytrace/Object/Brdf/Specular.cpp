#include "Object/Brdf/Specular.hpp"

#include "Parse/AST.h"

namespace Object {
namespace Brdf {

Specular::Specular(float strength)
{
	mStrength = strength;
}

bool Specular::specular() const
{
	return true;
}

Object::Color Specular::specularColor(const Object::Color &incidentColor, const Object::Color &albedo) const
{
	return incidentColor * mStrength;
}

std::unique_ptr<Specular> Specular::fromAst(AST *ast)
{
	return std::make_unique<Specular>(ast->data._float);
}

}
}