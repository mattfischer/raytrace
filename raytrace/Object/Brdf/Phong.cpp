#include "Object/Brdf/Phong.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"

#include <cmath>

namespace Object {
namespace Brdf {

Phong::Phong(float strength, float power)
{
	mStrength = strength;
	mPower = power;
}

Object::Color Phong::color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
{
	Math::Vector incident = -incidentDirection;
	Math::Vector reflect = incident + Math::Vector(normal) * (2 * (-normal * incident));

	float dot = reflect * outgoingDirection;
	float coeff = 0;
	if(dot > 0) {
		coeff = std::pow(dot, mPower);
	}

	return incidentColor * mStrength * coeff;
}

Phong *Phong::fromAst(AST *ast)
{
	return new Phong(ast->data._float, ast->children[0]->data._float);
}

}
}