#include "Object/Primitive/Model.hpp"

#include <fstream>

namespace Object {
namespace Primitive {

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
		mPatches.push_back(std::make_unique<BezierPatch>(std::move(controlPoints)));
	}
}

const std::vector<std::unique_ptr<BezierPatch>> &Model::patches() const
{
	return mPatches;
}

}
}