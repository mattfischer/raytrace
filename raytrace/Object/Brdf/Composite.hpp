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

	virtual Object::Radiance diffuseRadiance(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;
	virtual float lambert() const;

	virtual bool specular() const;
	virtual Object::Radiance specularRadiance(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;

private:
	std::vector<std::unique_ptr<Base>> mBrdfs;
};

}
}

#endif