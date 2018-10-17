#ifndef OBJECT_PRIMITIVE_GRID_HPP
#define OBJECT_PRIMITIVE_GRID_HPP

#include "Object/Primitive/Base.hpp"
#include "Math/Point.hpp"

#include <vector>

namespace Object {
namespace Primitive {

class Grid : public Base
{
public:
	Grid(int width, int height, std::vector<Math::Point> &&points);

protected:
	virtual float doIntersect(const Math::Ray &ray, Math::Normal &normal) const;
	virtual BoundingVolume doBoundingVolume() const;

private:
	struct BvhNode {
		int u;
		int v;
		BoundingVolume volume;

		std::vector<std::unique_ptr<BvhNode>> children;
	};

	float intersectBvhNode(const Math::Ray &ray, const BoundingVolume::RayData &raydata, const BvhNode &node, Math::Normal &normal) const;

	int mWidth;
	int mHeight;
	std::vector<Math::Point> mPoints;
	mutable std::unique_ptr<BvhNode> mBvhRoot;
};

}
}
#endif