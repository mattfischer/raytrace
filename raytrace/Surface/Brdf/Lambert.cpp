#include "Surface/Brdf/Lambert.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"

#include <cmath>

namespace Surface {
namespace Brdf {

Lambert::Lambert(float strength)
{
	mStrength = strength;
}

Object::Color Lambert::color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
{
	float dot = normal * incidentDirection;

	float coeff = 0;
	if(dot > 0) {
		coeff = dot;
	}

	return incidentColor * albedo * mStrength * coeff;
}

Lambert *Lambert::fromAst(AST *ast)
{
	return new Lambert(ast->data._float);
}

}
}