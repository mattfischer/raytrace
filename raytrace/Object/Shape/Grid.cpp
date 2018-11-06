#include "Object/Shape/Grid.hpp"
#include "Object/Shape/Triangle.hpp"

namespace Object {
	namespace Shape {
		Grid::Grid(int width, int height, std::vector<Vertex> &&vertices)
			: mWidth(width), mHeight(height), mVertices(std::move(vertices)), mBoundingVolumeHierarchy(computeBounds(0, 0, mWidth - 1, mHeight - 1))
		{
		}

		std::unique_ptr<BoundingVolumeHierarchy::Node> Grid::computeBounds(int uMin, int vMin, int uMax, int vMax) const
		{
			const int GROUP = 1;

			if (uMax - uMin <= GROUP && vMax - vMin <= GROUP) {
				std::unique_ptr<BvhNode> node = std::make_unique<BvhNode>();
				node->uMin = uMin;
				node->vMin = vMin;
				node->uMax = uMax;
				node->vMax = vMax;
				for (int i = uMin; i <= uMax; i++) {
					for (int j = vMin; j <= vMax; j++) {
						node->volume.expand(vertex(i, j).point);
					}
				}
				return node;
			}
			else {
				std::unique_ptr<BoundingVolumeHierarchy::Node> node = std::make_unique<BoundingVolumeHierarchy::Node>();
				if (uMax - uMin >= vMax - vMin) {
					int uSplit = (uMin + uMax) / 2;
					node->children[0] = computeBounds(uMin, vMin, uSplit, vMax);
					node->children[1] = computeBounds(uSplit, vMin, uMax, vMax);
				}
				else {
					int vSplit = (vMin + vMax) / 2;
					node->children[0] = computeBounds(uMin, vMin, uMax, vSplit);
					node->children[1] = computeBounds(uMin, vSplit, uMax, vMax);
				}

				node->volume.expand(node->children[0]->volume);
				node->volume.expand(node->children[1]->volume);

				return node;
			}
		}

		const Grid::Vertex &Grid::vertex(int u, int v) const
		{
			return mVertices[v * mWidth + u];
		}

		bool Grid::intersect(const Math::Ray &ray, Intersection &intersection) const
		{
			BoundingVolume::RayData rayData = BoundingVolume::getRayData(ray);

			auto callback = [&](const BoundingVolumeHierarchy::Node &node, float &maxDistance) {
				const BvhNode &bvhNode = static_cast<const BvhNode&>(node);
				bool ret = false;
				for (int u = bvhNode.uMin; u < bvhNode.uMax; u++) {
					for (int v = bvhNode.vMin; v < bvhNode.vMax; v++) {
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
					}
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