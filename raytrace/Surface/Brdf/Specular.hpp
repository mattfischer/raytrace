#ifndef SURFACE_BRDF_SPECULAR_HPP
#define SURFACE_BRDF_SPECULAR_HPP

#include "Surface/Brdf/Base.hpp"

namespace Surface {
namespace Brdf {

class Specular : public Base
{
public:
	Specular(float strength);

	virtual bool specular() const;
	virtual Object::Color specularColor(const Object::Color &incidentColor, const Object::Color &albedo) const;

	static Specular *fromAst(AST *ast);

private:
	float mStrength;
};

}
}

#endif