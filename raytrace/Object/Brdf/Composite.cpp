#include "Object/Brdf/Composite.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"

#include <vector>

namespace Object {
namespace Brdf {

Composite::Composite(const std::vector<const Base*> &brdfs)
: mBrdfs(brdfs)
{
}

Composite::~Composite()
{
	for(int i=0; i<mBrdfs.size(); i++) {
		delete mBrdfs[i];
	}
}

Object::Color Composite::color(const Object::Color &incidentColor, const Math::Vector &incidentDirection, const Math::Normal &normal, const Math::Vector &outgoingDirection, const Object::Color &albedo) const
{
	Object::Color totalColor;

	for(int i=0; i<mBrdfs.size(); i++) {
		totalColor = totalColor + mBrdfs[i]->color(incidentColor, incidentDirection, normal, outgoingDirection, albedo);
	}

	return totalColor;
}

bool Composite::specular() const
{
	for(int i=0; i<mBrdfs.size(); i++) {
		if(mBrdfs[i]->specular()) {
			return true;
		}
	}

	return false;
}

Object::Color Composite::specularColor(const Object::Color &incidentColor, const Object::Color &albedo) const
{
	Object::Color totalColor;

	for(int i=0; i<mBrdfs.size(); i++) {
		totalColor = totalColor + mBrdfs[i]->specularColor(incidentColor, albedo);
	}

	return totalColor;
}

}
}