#include "Object/Shape/Transformed.hpp"

namespace Object {
namespace Shape {

Transformed::Transformed(std::unique_ptr<Base> shape, const Math::Transformation &transformation)
	: mShape(std::move(shape)), mTransformation(transformation)
{
}

float Transformed::intersect(const Math::Ray &ray, Math::Normal &normal) const
{
	Math::Ray transformedRay = mTransformation.inverse() * ray;
	Math::Normal transformedNormal;
	float distance = mShape->intersect(transformedRay, transformedNormal);

	if (distance != FLT_MAX) {
		normal = (mTransformation * transformedNormal).normalize();
	}

	return distance;
}

BoundingVolume Transformed::boundingVolume(const Math::Transformation &transformation) const
{
	return mShape->boundingVolume(transformation * mTransformation);
}

bool Transformed::sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const
{
	Math::Point objectPoint;
	Math::Vector objectDu;
	Math::Vector objectDv;
	Math::Normal objectNormal;

	if (mShape->sample(u, v, objectPoint, objectDu, objectDv, objectNormal))
	{
		point = mTransformation * objectPoint;
		du = mTransformation * objectDu;
		dv = mTransformation * objectDv;
		normal = mTransformation * objectNormal;

		return true;
	}
	else {
		return false;
	}
}

}
}