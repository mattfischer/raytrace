#include "Object/Primitive/Sampleable.hpp"

namespace Object {
namespace Primitive {

void Sampleable::sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const
{
	Math::Point objectPoint;
	Math::Vector objectDu;
	Math::Vector objectDv;
	Math::Normal objectNormal;

	doSample(u, v, objectPoint, objectDu, objectDv, objectNormal);

	point = mTransformation * objectPoint;
	du = mTransformation * objectDu;
	dv = mTransformation * objectDv;
	normal = mTransformation * objectNormal;
}

}
}