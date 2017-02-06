#ifndef OBJECT_BRDF_BASE_HPP
#define OBJECT_BRDF_BASE_HPP

#include "Math/Forwards.hpp"

#include "Object/Color.hpp"

namespace Object {
namespace Brdf {

class Base {
public:
	virtual ~Base() {}

	virtual Object::Color color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;

	virtual bool specular() const;
	virtual Object::Color specularColor(const Object::Color &incidentColor, const Object::Color &albedo) const;

	static const Base *fromAst(AST *ast);
};

}
}

#endif