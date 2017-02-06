#ifndef OBJECT_BRDF_AMBIENT_HPP
#define OBJECT_BRDF_AMBIENT_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
namespace Brdf {

class Ambient : public Base
{
public:
	Ambient(float strength);

	virtual Object::Color color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;

	static Ambient *fromAst(AST *ast);

private:
	float mStrength;
};

}
}

#endif