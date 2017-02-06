#ifndef OBJECT_BRDF_PHONG_HPP
#define OBJECT_BRDF_PHONG_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
namespace Brdf {

class Phong : public Base
{
public:
	Phong(float strength, float power);

	virtual Object::Color color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;

	static Phong *fromAst(AST *ast);

private:
	float mStrength;
	float mPower;
};

}
}

#endif