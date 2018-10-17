#include "Object/Shape/Grid.hpp"

namespace Object {
namespace Shape {

Grid::Grid(int width, int height, std::vector<Math::Point> &&points)
	: mWidth(width), mHeight(height), mPoints(std::move(points))
{
	std::vector<std::unique_ptr<BvhNode>> nodes;

	for (int v = 0; v < mHeight - 1; v++) {
		for (int u = 0; u < mWidth - 1; u++) {
			std::unique_ptr<BvhNode> node = std::make_unique<BvhNode>();

			node->u = u;
			node->v = v;
			node->volume.expand(mPoints[v * mWidth + u]);
			node->volume.expand(mPoints[v * mWidth + u + 1]);
			node->volume.expand(mPoints[(v + 1) * mWidth + u]);
			node->volume.expand(mPoints[(v + 1) * mWidth + u + 1]);

			nodes.push_back(std::move(node));
		}
	}

	int currentWidth = mWidth - 1;
	int currentHeight = mHeight - 1;

	while (currentWidth > 1 || currentHeight > 1) {
		std::vector<std::unique_ptr<BvhNode>> newNodes;
		for (int v = currentHeight - 2; v >= -1; v -= 2) {
			for (int u = currentWidth - 2; u >= -1; u -= 2) {
				std::unique_ptr<BvhNode> newNode;
				if (u == -1 && v == -1) {
					newNode = std::move(nodes[0]);
				}
				else {
					newNode = std::make_unique<BvhNode>();
					newNode->u = newNode->v = -1;
					for (int i = 0; i < 2; i++) {
						for (int j = 0; j < 2; j++) {
							int uu = u + i;
							int vv = v + j;
							if (uu == -1 || vv == -1) {
								continue;
							}
							std::unique_ptr<BvhNode> node = std::move(nodes[vv * currentWidth + uu]);
							newNode->volume.expand(node->volume);
							newNode->children.push_back(std::move(node));
						}
					}
				}
				newNodes.push_back(std::move(newNode));
			}
		}

		nodes = std::move(newNodes);
		currentWidth = (currentWidth + 1) / 2;
		currentHeight = (currentHeight + 1) / 2;
	}

	mBvhRoot = std::move(nodes[0]);
}

bool intersectTriangle(const Math::Ray &ray, const Math::Point &point0, const Math::Point &point1, const Math::Point &point2, float &distance, Math::Normal &normal)
{
	Math::Vector E1 = point1 - point0;
	Math::Vector E2 = point2 - point0;
	Math::Vector P = ray.direction() % E2;

	float den = P * E1;
	if (den > -0.001 && den < 0.001) {
		return false;
	}

	Math::Vector T = ray.origin() - point0;
	float u = (P * T) / den;
	if (u < 0 || u > 1) {
		return false;
	}

	Math::Vector Q = T % E1;
	float v = (Q * ray.direction()) / den;
	if (v < 0 || u + v > 1) {
		return false;
	}

	distance = (Q * E2) / den;
	if (distance < 0) {
		return false;
	}

	normal = Math::Normal(E1 % E2).normalize();
	return true;
}

float Grid::intersect(const Math::Ray &ray, Math::Normal &normal) const
{
	BoundingVolume::RayData rayData = BoundingVolume::getRayData(ray);
	return intersectBvhNode(ray, rayData, *mBvhRoot, normal);
}

BoundingVolume Grid::boundingVolume(const Math::Transformation &transformation) const
{
	BoundingVolume volume;
	for (const Math::Point &point : mPoints) {
		volume.expand(transformation * point);
	}

	return volume;
}

float Grid::intersectBvhNode(const Math::Ray &ray, const BoundingVolume::RayData &rayData, const BvhNode &node, Math::Normal &normal) const
{
	float distance = FLT_MAX;

	float newDistance;
	Math::Normal newNormal;
	if (node.u != -1 && node.v != -1) {
		const Math::Point point0 = mPoints[node.v * mWidth + node.u];
		const Math::Point point1 = mPoints[node.v * mWidth + node.u + 1];
		const Math::Point point2 = mPoints[(node.v + 1) * mWidth + node.u];
		const Math::Point point3 = mPoints[(node.v + 1) * mWidth + node.u + 1];

		if (intersectTriangle(ray, point0, point1, point2, newDistance, newNormal)) {
			distance = newDistance;
			normal = newNormal;
		} else if (intersectTriangle(ray, point3, point2, point1, newDistance, newNormal)) {
			distance = newDistance;
			normal = newNormal;
		}
	}
	else {
		for (const std::unique_ptr<BvhNode> &child : node.children) {
			float volumeDistance;
			if (child->volume.intersectRay(rayData, volumeDistance) && volumeDistance < distance) {
				newDistance = intersectBvhNode(ray, rayData, *child, newNormal);
				if (newDistance < distance) {
					distance = newDistance;
					normal = newNormal;
				}
			}
		}
	}

	return distance;
}

bool Grid::sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const
{
	return false;
}

}
}