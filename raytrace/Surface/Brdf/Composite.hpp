#ifndef SURFACE_BRDF_COMPOSITE_HPP
#define SURFACE_BRDF_COMPOSITE_HPP

#include "Surface/Brdf/Base.hpp"

#include <vector>

namespace Surface {
namespace Brdf {

class Composite : public Base
{
public:
	Composite(const std::vector<const Base*> &brdfs);
	virtual ~Composite();

	virtual Object::Color color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const;

	virtual bool specular() const;
	virtual Object::Color specularColor(const Object::Color &incidentColor, const Object::Color &albedo) const;

private:
	std::vector<const Base*> mBrdfs;
};

}
}

#endif