#include "Object/Shape/Grid.hpp"
#include "Object/Shape/Triangle.hpp"

namespace Object {
	namespace Shape {
		Grid::Grid(int width, int height, std::vector<Vertex> &&vertices)
			: mWidth(width), mHeight(height), mVertices(std::move(vertices))
		{
			std::vector<Object::BoundingVolumeHierarchy::Node> nodes;
			nodes.reserve(mWidth * mHeight * 2);
			computeBounds(nodes, 0, 0, mWidth - 1, mHeight - 1);

			mBoundingVolumeHierarchy = Object::BoundingVolumeHierarchy(std::move(nodes));
		}

		int Grid::computeBounds(std::vector<Object::BoundingVolumeHierarchy::Node> &nodes, int uMin, int vMin, int uMax, int vMax) const
		{
			nodes.push_back(Object::BoundingVolumeHierarchy::Node());
			int nodeIndex = nodes.size() - 1;
			Object::BoundingVolumeHierarchy::Node &node = nodes[nodeIndex];

			if (uMax - uMin == 1 && vMax - vMin == 1) {
				node.index = -(vMin * mWidth + uMin);
				for (int i = uMin; i <= uMax; i++) {
					for (int j = vMin; j <= vMax; j++) {
						node.volume.expand(vertex(i, j).point);
					}
				}
			}
			else {
				if (uMax - uMin >= vMax - vMin) {
					int uSplit = (uMin + uMax) / 2;
					computeBounds(nodes, uMin, vMin, uSplit, vMax);
					node.index = computeBounds(nodes, uSplit, vMin, uMax, vMax);
				}
				else {
					int vSplit = (vMin + vMax) / 2;
					computeBounds(nodes, uMin, vMin, uMax, vSplit);
					node.index = computeBounds(nodes, uMin, vSplit, uMax, vMax);
				}

				node.volume.expand(nodes[nodeIndex + 1].volume);
				node.volume.expand(nodes[node.index].volume);
			}

			return nodeIndex;
		}

		const Grid::Vertex &Grid::vertex(int u, int v) const
		{
			return mVertices[v * mWidth + u];
		}

		bool Grid::intersect(const Math::Ray &ray, Intersection &intersection) const
		{
			BoundingVolume::RayData rayData = BoundingVolume::getRayData(ray);

			auto callback = [&](int index, float &maxDistance) {
				bool ret = false;
				int u = index % mWidth;
				int v = index / mWidth;
				const Vertex &vertex0 = vertex(u, v);
				Math::Point2D surfacePoint0((float)u / mWidth, (float)v / mHeight);
				const Vertex &vertex1 = vertex(u + 1, v);
				Math::Point2D surfacePoint1((float)(u + 1) / mWidth, (float)v / mHeight);
				const Vertex &vertex2 = vertex(u, v + 1);
				Math::Point2D surfacePoint2((float)u / mWidth, (float)(v + 1) / mHeight);
				const Vertex &vertex3 = vertex(u + 1, v + 1);
				Math::Point2D surfacePoint3((float)(u + 1) / mWidth, (float)(v + 1) / mHeight);

				float tu, tv;
				if (Triangle::intersect(ray, vertex0.point, vertex1.point, vertex2.point, intersection.distance, tu, tv)) {
					intersection.normal = vertex0.normal * (1 - tu - tv) + vertex1.normal * tu + vertex2.normal * tv;
					intersection.tangent = vertex0.tangent * (1 - tu - tv) + vertex1.tangent * tu + vertex2.tangent * tv;
					intersection.surfacePoint = surfacePoint0 * (1 - tu - tv) + surfacePoint1 * tu + surfacePoint2 * tv;
					ret = true;
				}
				if (Triangle::intersect(ray, vertex3.point, vertex2.point, vertex1.point, intersection.distance, tu, tv)) {
					intersection.normal = vertex3.normal * (1 - tu - tv) + vertex2.normal * tu + vertex1.normal * tv;
					intersection.tangent = vertex3.tangent * (1 - tu - tv) + vertex2.tangent * tu + vertex1.tangent * tv;
					intersection.surfacePoint = surfacePoint3 * (1 - tu - tv) + surfacePoint2 * tu + surfacePoint1 * tv;
					ret = true;
				}

				return ret;
			};

			return mBoundingVolumeHierarchy.intersect(rayData, intersection.distance, std::ref(callback));
		}

		BoundingVolume Grid::boundingVolume(const Math::Transformation &transformation) const
		{
			BoundingVolume volume;
			for (const Vertex &vertex : mVertices) {
				volume.expand(transformation * vertex.point);
			}

			return volume;
		}
	}
}