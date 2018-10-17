#define _USE_MATH_DEFINES
#include "Object/Brdf/Phong.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"

#include "Lighter/Utils.hpp"

#include <cmath>

namespace Object {
namespace Brdf {

Phong::Phong(float strength, float power)
{
	mStrength = strength;
	mPower = power;
}

Object::Radiance Phong::radiance(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
{
	Math::Vector reflectDirection = -(incidentDirection - Math::Vector(normal) * (2 * (normal * incidentDirection)));

	float dot = reflectDirection * outgoingDirection;
	float coeff = 0;
	if(dot > 0) {
		coeff = std::pow(dot, mPower);
	}

	return incidentRadiance * mStrength * coeff * (mPower + 1) / (2 * M_PI);
}

Math::Vector Phong::sample(float u, float v, const Math::Normal &normal, const Math::Vector &outgoingDirection, float &pdf) const
{
	float phi = 2 * M_PI * u;
	float theta = std::acos(std::pow(v, 1.0f / (mPower + 1)));

	Math::Vector x, y;
	Lighter::Utils::orthonormalBasis(outgoingDirection, x, y);

	Math::Vector direction = x * std::cos(phi) * std::sin(theta) + y * std::sin(phi) * std::sin(theta) + outgoingDirection * std::cos(theta);
	Math::Vector incidentDirection = -(direction - Math::Vector(normal) * (direction * normal * 2));

	float coeff = 0;
	float dot = direction * outgoingDirection;
	if (dot > 0) {
		coeff = std::pow(dot, mPower);
	}

	pdf = coeff * (mPower + 1) / (2 * M_PI);

	return incidentDirection;
}

std::unique_ptr<Phong> Phong::fromAst(AST *ast)
{
	float strength = ast->children[0]->data._float;
	float power = ast->children[1]->data._float;
	return std::make_unique<Phong>(strength, power);
}

}
}