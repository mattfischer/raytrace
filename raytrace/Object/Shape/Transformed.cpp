#include "Object/Shape/Transformed.hpp"

namespace Object {
namespace Shape {

Transformed::Transformed(std::unique_ptr<Base> shape, const Math::Transformation &transformation)
	: mShape(std::move(shape)), mTransformation(transformation)
{
}

bool Transformed::intersect(const Math::Ray &ray, float &distance, Math::Normal &normal) const
{
	Math::Ray transformedRay = mTransformation.inverse() * ray;
	Math::Normal transformedNormal;
	if (mShape->intersect(transformedRay, distance, transformedNormal)) {
		normal = (mTransformation * transformedNormal).normalize();
		return true;
	}

	return false;
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