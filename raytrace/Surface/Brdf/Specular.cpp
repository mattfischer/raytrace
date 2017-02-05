#include "Surface/Brdf/Specular.hpp"

#include "Parse/AST.h"

namespace Surface {
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

Specular *Specular::fromAst(AST *ast)
{
	return new Specular(ast->data._float);
}

}
}