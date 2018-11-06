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
				int uMin;
				int vMin;
				int uMax;
				int vMax;
			};

			std::unique_ptr<BoundingVolumeHierarchy::Node> computeBounds(int u, int v, int du, int dv) const;
			const Vertex &vertex(int u, int v) const;

			int mWidth;
			int mHeight;
			std::vector<Vertex> mVertices;
			Object::BoundingVolumeHierarchy mBoundingVolumeHierarchy;
		};
	}
}
#endif