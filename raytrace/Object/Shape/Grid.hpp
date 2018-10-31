#ifndef OBJECT_SHAPE_GRID_HPP
#define OBJECT_SHAPE_GRID_HPP

#include "Object/Shape/Base.hpp"
#include "Math/Point.hpp"
#include "Math/Bivector.hpp"

#include <vector>
#include <functional>

namespace Object {
	namespace Shape {
		class Grid : public Base
		{
		public:
			Grid(int width, int height, std::vector<Math::Point> &&points, std::vector<Math::Normal> &&normals, std::vector<Math::Bivector> &&tangents);

			virtual bool intersect(const Math::Ray &ray, Intersection &intersection) const;
			virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const;

		private:
			struct BvhNode {
				int u;
				int v;
				int du;
				int dv;
				BoundingVolume volume;

				std::vector<std::unique_ptr<BvhNode>> children;
			};

			bool intersectTriangle(const Math::Ray &ray, int idx0, int idx1, int idx2, Shape::Base::Intersection &intersection) const;
			bool intersectBvhNode(const BoundingVolume::RayData &raydata, const BvhNode &node, Intersection &intersection, const std::function<bool(int, int, Shape::Base::Intersection &intersection)> &func) const;

			int mWidth;
			int mHeight;
			std::vector<Math::Point> mPoints;
			std::vector<Math::Normal> mNormals;
			std::vector<Math::Bivector> mTangents;
			mutable std::unique_ptr<BvhNode> mBvhRoot;
		};
	}
}
#endif