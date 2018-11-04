#include "Object/Shape/Grid.hpp"

namespace Object {
	namespace Shape {
		Grid::Grid(int width, int height, std::vector<Vertex> &&vertices)
			: mWidth(width), mHeight(height), mVertices(std::move(vertices)), mBoundingVolumeHierarchy(computeBoundingVolumeHierarchy())
		{
		}

		std::unique_ptr<BoundingVolumeHierarchy::Node> Grid::computeBoundingVolumeHierarchy() const
		{
			std::vector<std::unique_ptr<BoundingVolumeHierarchy::Node>> nodes;

			const int GROUP = 1;
			for (int v = 0; v < mHeight - 1; v += GROUP) {
				for (int u = 0; u < mWidth - 1; u += GROUP) {
					std::unique_ptr<BvhNode> node = std::make_unique<BvhNode>();

					node->u = u;
					node->v = v;
					node->du = std::min(GROUP, mWidth - 1 - u);
					node->dv = std::min(GROUP, mHeight - 1 - v);

					for (int i = 0; i < node->du + 1; i++) {
						for (int j = 0; j < node->dv + 1; j++) {
							node->volume.expand(mVertices[(v + j) * mWidth + u + i].point);
						}
					}

					nodes.push_back(std::move(node));
				}
			}

			int currentWidth = (mWidth - 1 + GROUP - 1) / GROUP;
			int currentHeight = (mHeight - 1 + GROUP - 1) / GROUP;

			while (currentWidth > 1 || currentHeight > 1) {
				std::vector<std::unique_ptr<BoundingVolumeHierarchy::Node>> newNodes;
				for (int v = currentHeight - 2; v >= -1; v -= 2) {
					for (int u = currentWidth - 2; u >= -1; u -= 2) {
						std::unique_ptr<BoundingVolumeHierarchy::Node> newNodeU;
						if (u != -1) {
							newNodeU = std::make_unique<BoundingVolumeHierarchy::Node>();
						}

						for (int i = 0; i < 2; i++) {
							int uu = u + i;
							if (uu == -1) {
								continue;
							}

							std::unique_ptr<BoundingVolumeHierarchy::Node> newNodeV;
							if (v == -1) {
								newNodeV = std::move(nodes[0 * currentWidth + uu]);
							}
							else {
								newNodeV = std::make_unique<BoundingVolumeHierarchy::Node>();
								for (int j = 0; j < 2; j++) {
									int vv = v + j;
									std::unique_ptr<BoundingVolumeHierarchy::Node> node = std::move(nodes[vv * currentWidth + uu]);
									newNodeV->volume.expand(node->volume);
									newNodeV->children[j] = std::move(node);
								}
							}

							if (u == -1) {
								newNodeU = std::move(newNodeV);
							}
							else {
								newNodeU->volume.expand(newNodeV->volume);
								newNodeU->children[i] = std::move(newNodeV);
							}
						}
						newNodes.push_back(std::move(newNodeU));
					}
				}

				nodes = std::move(newNodes);
				currentWidth = (currentWidth + 1) / 2;
				currentHeight = (currentHeight + 1) / 2;
			}

			return std::move(nodes[0]);
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
				for (int u = bvhNode.u; u < bvhNode.u + bvhNode.du; u++) {
					for (int v = bvhNode.v; v < bvhNode.v + bvhNode.dv; v++) {
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