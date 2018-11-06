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

		bool Grid::intersectTriangle(const Math::Ray &ray, const Math::Point &p, const Math::Point &pu, const Math::Point &pv, float &distance, float &u, float &v) const
		{
			Math::Vector E1 = pu - p;
			Math::Vector E2 = pv - p;
			Math::Vector P = ray.direction() % E2;

			float den = P * E1;
			if (den > -0.000001 && den < 0.000001) {
				return false;
			}

			float iden = 1.0f / den;

			Math::Vector T = ray.origin() - p;
			float uu = (P * T) * iden;
			if (uu < 0 || uu > 1) {
				return false;
			}

			Math::Vector Q = T % E1;
			float vv = (Q * ray.direction()) * iden;
			if (vv < 0 || uu + vv > 1) {
				return false;
			}

			float d = (Q * E2) * iden;
			if (d < 0 || d >= distance) {
				return false;
			}

			distance = d;
			u = uu;
			v = vv;
			return true;
		}

		bool Grid::intersect(const Math::Ray &ray, Intersection &intersection) const
		{
			BoundingVolume::RayData rayData = BoundingVolume::getRayData(ray);

			auto callback = [&](const BoundingVolumeHierarchy::Node &node, float &maxDistance) {
				const BvhNode &bvhNode = static_cast<const BvhNode&>(node);
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
						bool ret = false;
						if (intersectTriangle(ray, vertex0.point, vertex1.point, vertex2.point, intersection.distance, tu, tv)) {
							intersection.normal = vertex0.normal * (1 - tu - tv) + vertex1.normal * tu + vertex2.normal * tv;
							intersection.tangent = vertex0.tangent * (1 - tu - tv) + vertex1.tangent * tu + vertex2.tangent * tv;
							intersection.surfacePoint = surfacePoint0 * (1 - tu - tv) + surfacePoint1 * tu + surfacePoint2 * tv;
							ret = true;
						}
						if (intersectTriangle(ray, vertex3.point, vertex2.point, vertex1.point, intersection.distance, tu, tv)) {
							intersection.normal = vertex3.normal * (1 - tu - tv) + vertex2.normal * tu + vertex1.normal * tv;
							intersection.tangent = vertex3.tangent * (1 - tu - tv) + vertex2.tangent * tu + vertex1.tangent * tv;
							intersection.surfacePoint = surfacePoint3 * (1 - tu - tv) + surfacePoint2 * tu + surfacePoint1 * tv;
							ret = true;
						}

						if (ret) {
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