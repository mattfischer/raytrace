#include "Object/Shape/Grid.hpp"

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

		bool Grid::intersectTriangle(const Math::Ray &ray, int idx0, int idx1, int idx2, Shape::Base::Intersection &intersection) const
		{
			const Vertex &vertex0 = mVertices[idx0];
			Math::Point2D surfacePoint0 = Math::Point2D((float)(idx0 % mWidth) / mWidth, (float)(idx0 / mWidth) / mHeight);
			const Vertex &vertex1 = mVertices[idx1];
			Math::Point2D surfacePoint1 = Math::Point2D((float)(idx1 % mWidth) / mWidth, (float)(idx1 / mWidth) / mHeight);
			const Vertex &vertex2 = mVertices[idx2];
			Math::Point2D surfacePoint2 = Math::Point2D((float)(idx2 % mWidth) / mWidth, (float)(idx2 / mWidth) / mHeight);

			Math::Vector E1 = vertex1.point - vertex0.point;
			Math::Vector E2 = vertex2.point - vertex0.point;
			Math::Vector P = ray.direction() % E2;

			float den = P * E1;
			if (den > -0.000001 && den < 0.000001) {
				return false;
			}

			float iden = 1.0f / den;

			Math::Vector T = ray.origin() - vertex0.point;
			float u = (P * T) * iden;
			if (u < 0 || u > 1) {
				return false;
			}

			Math::Vector Q = T % E1;
			float v = (Q * ray.direction()) * iden;
			if (v < 0 || u + v > 1) {
				return false;
			}

			float distance = (Q * E2) * iden;
			if (distance < 0 || distance >= intersection.distance) {
				return false;
			}

			intersection.distance = distance;
			intersection.normal = vertex0.normal * (1 - u - v) + vertex1.normal * u + vertex2.normal * v;
			intersection.surfacePoint = surfacePoint0 * (1 - u - v) + surfacePoint1 * u + surfacePoint2 * v;
			intersection.tangent = vertex0.tangent * (1 - u - v) + vertex1.tangent * u + vertex2.tangent * v;
			return true;
		}

		bool Grid::intersect(const Math::Ray &ray, Intersection &intersection) const
		{
			BoundingVolume::RayData rayData = BoundingVolume::getRayData(ray);

			auto callback = [&](const BoundingVolumeHierarchy::Node &node, float &maxDistance) {
				const BvhNode &bvhNode = static_cast<const BvhNode&>(node);
				for (int u = bvhNode.uMin; u < bvhNode.uMax; u++) {
					for (int v = bvhNode.vMin; v < bvhNode.vMax; v++) {
						int idx0 = v * mWidth + u;
						int idx1 = v * mWidth + u + 1;
						int idx2 = (v + 1) * mWidth + u;
						int idx3 = (v + 1) * mWidth + u + 1;

						if (intersectTriangle(ray, idx0, idx1, idx2, intersection)) {
							return true;
						}
						else if (intersectTriangle(ray, idx3, idx2, idx1, intersection)) {
							return true;
						}
					}
				}
				return false;
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