#ifndef SURFACE_BRDF_AMBIENT_HPP
#define SURFACE_BRDF_AMBIENT_HPP

#include "Surface/Brdf/Base.hpp"

namespace Surface {
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