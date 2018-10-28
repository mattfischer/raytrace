#include "Object/Shape/Model.hpp"

#include "Object/Shape/BezierPatch.hpp"

#include <fstream>

namespace Object {
	namespace Shape {
		Model::Model(const std::string &filename)
		{
			std::vector<std::unique_ptr<Base>> patches;

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
				patches.push_back(std::make_unique<BezierPatch>(16, 16, std::move(controlPoints)));
			}

			mGroup = std::make_unique<Group>(std::move(patches));
		}


		bool Model::intersect(const Math::Ray &ray, Intersection &intersection) const
		{
			return mGroup->intersect(ray, intersection);
		}

		BoundingVolume Model::boundingVolume(const Math::Transformation &transformation) const
		{
			return mGroup->boundingVolume(transformation);
		}
	}
}