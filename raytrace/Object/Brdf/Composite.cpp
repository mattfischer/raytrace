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

Object::Radiance Composite::diffuseRadiance(const Object::Radiance &incidentRadiance, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
{
	Object::Radiance totalRadiance;

	for(const std::unique_ptr<Brdf::Base> &brdf : mBrdfs) {
		totalRadiance = totalRadiance + brdf->diffuseRadiance(incidentRadiance, incidentDirection, normal, outgoingDirection, albedo);
	}

	return totalRadiance;
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

float Composite::lambert() const
{
	float lambert = 0;

	for (const std::unique_ptr<Brdf::Base> &brdf : mBrdfs) {
		lambert += brdf->lambert();
	}

	return lambert;
}

Object::Radiance Composite::specularRadiance(const Object::Radiance &incidentRadiance, const Object::Color &albedo) const
{
	Object::Radiance totalRadiance;

	for (const std::unique_ptr<Brdf::Base> &brdf : mBrdfs) {
		totalRadiance = totalRadiance + brdf->specularRadiance(incidentRadiance, albedo);
	}

	return totalRadiance;
}

}
}