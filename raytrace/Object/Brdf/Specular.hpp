#ifndef OBJECT_BRDF_SPECULAR_HPP
#define OBJECT_BRDF_SPECULAR_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
namespace Brdf {

class Specular : public Base
{
public:
	Specular(float strength);

	virtual bool specular() const;
	virtual Object::Radiance specularRadiance(const Object::Radiance &incidentRadiance, const Object::Color &albedo) const;

	static std::unique_ptr<Specular> fromAst(AST *ast);

private:
	float mStrength;
};

}
}

#endif