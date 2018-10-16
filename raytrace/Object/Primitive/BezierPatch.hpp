#ifndef OBJECT_PRIMITIVE_BEZIER_PATCH_HPP
#define OBJECT_PRIMITIVE_BEZIER_PATCH_HPP

#include "Object/Primitive/Base.hpp"
#include "Object/Primitive/Grid.hpp"

#include <vector>

namespace Object {
namespace Primitive {

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