#ifndef OBJECT_SHAPE_BEZIER_PATCH_HPP
#define OBJECT_SHAPE_BEZIER_PATCH_HPP

#include "Object/Shape/Base.hpp"
#include "Object/Shape/Grid.hpp"

#include <vector>

namespace Object {
namespace Shape {

class BezierPatch {
public:
	BezierPatch(std::vector<Math::Point> &&controlPoints);

	std::unique_ptr<Grid> tesselate(int width, int height) const;

private:
	std::vector<Math::Point> mControlPoints;
};
}
}
#endif