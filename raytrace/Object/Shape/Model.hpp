#ifndef OBJECT_SHAPE_MODEL_HPP
#define OBJECT_SHAPE_MODEL_HPP

#include "Object/Shape/BezierPatch.hpp"

namespace Object {
namespace Shape {

class Model
{
public:
	Model(const std::string &filename);

	const std::vector<std::unique_ptr<BezierPatch>> &patches() const;

private:
	std::vector<std::unique_ptr<BezierPatch>> mPatches;
};

}
}
#endif