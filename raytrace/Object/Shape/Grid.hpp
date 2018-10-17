#ifndef OBJECT_SHAPE_GRID_HPP
#define OBJECT_SHAPE_GRID_HPP

#include "Object/Shape/Base.hpp"
#include "Math/Point.hpp"

#include <vector>

namespace Object {
namespace Shape {

class Grid : public Base
{
public:
	Grid(int width, int height, std::vector<Math::Point> &&points);

	virtual float intersect(const Math::Ray &ray, Math::Normal &normal) const;
	virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const;
	virtual bool sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const;

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