#include "Object/Shape/Group.hpp"

namespace Object {
namespace Shape {

Group::Group(std::vector<std::unique_ptr<Base>> &&shapes)
	: mShapes(std::move(shapes))
{
	Math::Transformation transformation;

	for (const std::unique_ptr<Base> &shape : mShapes) {
		mVolumes.push_back(shape->boundingVolume(transformation));
	}
}

bool Group::intersect(const Math::Ray &ray, Intersection &intersection) const
{
	bool ret = false;

	BoundingVolume::RayData rayData = BoundingVolume::getRayData(ray);
	for(int i=0; i<mShapes.size(); i++) {
		float distance;
		if (mVolumes[i].intersectRay(rayData, distance) && distance < intersection.distance) {
			if (mShapes[i]->intersect(ray, intersection)) {
				ret = true;
			}
		}
	}

	return ret;
}

BoundingVolume Group::boundingVolume(const Math::Transformation &transformation) const
{
	BoundingVolume volume;

	for (const std::unique_ptr<Base> &shape : mShapes) {
		volume.expand(shape->boundingVolume(transformation));
	}

	return volume;
}

bool Group::sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const
{
	return false;
}

}
}