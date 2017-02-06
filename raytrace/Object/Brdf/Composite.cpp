#include "Object/Brdf/Composite.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"

#include <vector>

namespace Object {
namespace Brdf {

Composite::Composite(std::vector<std::unique_ptr<Base>> &&brdfs)
: mBrdfs(std::move(brdfs))
{
}

Object::Color Composite::color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
{
	Object::Color totalColor;

	for(const std::unique_ptr<Brdf::Base> &brdf : mBrdfs) {
		totalColor = totalColor + brdf->color(incidentColor, incidentDirection, normal, outgoingDirection, albedo);
	}

	return totalColor;
}

bool Composite::specular() const
{
	for (const std::unique_ptr<Brdf::Base> &brdf : mBrdfs) {
		if(brdf->specular()) {
			return true;
		}
	}

	return false;
}

Object::Color Composite::specularColor(const Object::Color &incidentColor, const Object::Color &albedo) const
{
	Object::Color totalColor;

	for (const std::unique_ptr<Brdf::Base> &brdf : mBrdfs) {
		totalColor = totalColor + brdf->specularColor(incidentColor, albedo);
	}

	return totalColor;
}

}
}