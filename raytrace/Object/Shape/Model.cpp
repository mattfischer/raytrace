#include "Object/Shape/Model.hpp"

#include <fstream>

namespace Object {
namespace Shape {

Model::Model(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	int numPatches;

	file >> numPatches;
	for (int i = 0; i < numPatches; i++) {
		int dimx, dimy;
		file >> dimx >> dimy;
		std::vector<Math::Point> controlPoints;
		for (int j = 0; j < 16; j++) {
			float x, y, z;
			file >> x >> y >> z;
			controlPoints.push_back(Math::Point(x, y, z));
		}
		mPatches.push_back(std::make_unique<BezierPatch>(16, 16, std::move(controlPoints)));
	}
}


float Model::intersect(const Math::Ray &ray, Math::Normal &normal) const
{
	float distance = FLT_MAX;

	float newDistance;
	Math::Normal newNormal;
	for (const std::unique_ptr<BezierPatch> &patch : mPatches) {
		newDistance = patch->intersect(ray, newNormal);
		if (newDistance < distance) {
			distance = newDistance;
			normal = newNormal;
		}
	}

	return distance;
}

BoundingVolume Model::boundingVolume(const Math::Transformation &transformation) const
{
	BoundingVolume volume;

	for (const std::unique_ptr<BezierPatch> &patch : mPatches) {
		volume.expand(patch->boundingVolume(transformation));
	}

	return volume;
}

bool Model::sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const
{
	return false;
}

}
}