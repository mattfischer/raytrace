#ifndef OBJECT_BRDF_LAMBERT_HPP
#define OBJECT_BRDF_LAMBERT_HPP

#include "Object/Brdf/Base.hpp"

namespace Object {
namespace Brdf {

class Lambert : public Base
{
public:
	Lambert(float strength);

	virtual Object::Radiance diffuseRadiance(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;
	virtual float lambert() const;

	static std::unique_ptr<Lambert> fromAst(AST *ast);

private:
	float mStrength;
};

}
}

#endif