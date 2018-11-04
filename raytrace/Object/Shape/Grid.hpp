#ifndef OBJECT_SHAPE_GRID_HPP
#define OBJECT_SHAPE_GRID_HPP

#include "Object/Shape/Base.hpp"
#include "Object/BoundingVolumeHierarchy.hpp"

#include "Math/Point.hpp"
#include "Math/Bivector.hpp"

#include <vector>
#include <functional>

namespace Object {
	namespace Shape {
		class Grid : public Base
		{
		public:
			struct Vertex {
				Math::Point point;
				Math::Normal normal;
				Math::Bivector tangent;
			};

			Grid(int width, int height, std::vector<Vertex> &&vertices);

			virtual bool intersect(const Math::Ray &ray, Intersection &intersection) const;
			virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const;

		private:
			struct BvhNode : BoundingVolumeHierarchy::Node {
				int u;
				int v;
				int du;
				int dv;
			};

			bool intersectTriangle(const Math::Ray &ray, int idx0, int idx1, int idx2, Shape::Base::Intersection &intersection) const;
			std::unique_ptr<BoundingVolumeHierarchy::Node> computeBoundingVolumeHierarchy() const;

			int mWidth;
			int mHeight;
			std::vector<Vertex> mVertices;
			Object::BoundingVolumeHierarchy mBoundingVolumeHierarchy;
		};
	}
}
#endif