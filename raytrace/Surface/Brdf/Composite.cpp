#include "Surface/Brdf/Composite.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"

#include <vector>

namespace Surface {
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

}
}