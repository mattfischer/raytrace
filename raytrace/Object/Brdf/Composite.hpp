#ifndef OBJECT_BRDF_COMPOSITE_HPP
#define OBJECT_BRDF_COMPOSITE_HPP

#include "Object/Brdf/Base.hpp"

#include <vector>

namespace Object {
namespace Brdf {

class Composite : public Base
{
public:
	Composite(std::vector<std::unique_ptr<Base>> &&brdfs);

	virtual Object::Color color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;

	virtual bool specular() const;
	virtual Object::Color specularColor(const Object::Color &incidentColor, const Object::Color &albedo) const;

private:
	std::vector<std::unique_ptr<Base>> mBrdfs;
};

}
}

#endif