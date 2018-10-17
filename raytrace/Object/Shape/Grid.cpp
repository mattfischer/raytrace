#include "Object/Shape/Grid.hpp"

namespace Object {
namespace Shape {

Grid::Grid(int width, int height, std::vector<Math::Point> &&points, std::vector<Math::Normal> &&normals)
	: mWidth(width), mHeight(height), mPoints(std::move(points)), mNormals(std::move(normals))
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

bool Grid::intersectTriangle(const Math::Ray &ray, int idx0, int idx1, int idx2, Shape::Base::Intersection &intersection) const
{
	const Math::Point &point0 = mPoints[idx0];
	const Math::Point &point1 = mPoints[idx1];
	const Math::Point &point2 = mPoints[idx2];

	Math::Vector E1 = point1 - point0;
	Math::Vector E2 = point2 - point0;
	Math::Vector P = ray.direction() % E2;

	float den = P * E1;
	if (den > -0.000001 && den < 0.000001) {
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

	float distance = (Q * E2) / den;
	if (distance < 0 || distance >= intersection.distance) {
		return false;
	}

	intersection.distance = distance;
	intersection.normal = mNormals[idx0] * (1 - u - v) + mNormals[idx1] * u + mNormals[idx2] * v;
	return true;
}

bool Grid::intersect(const Math::Ray &ray, Intersection &intersection) const
{
	BoundingVolume::RayData rayData = BoundingVolume::getRayData(ray);
	return intersectBvhNode(ray, rayData, *mBvhRoot, intersection);
}

BoundingVolume Grid::boundingVolume(const Math::Transformation &transformation) const
{
	BoundingVolume volume;
	for (const Math::Point &point : mPoints) {
		volume.expand(transformation * point);
	}

	return volume;
}

bool Grid::intersectBvhNode(const Math::Ray &ray, const BoundingVolume::RayData &rayData, const BvhNode &node, Intersection &intersection) const
{
	bool ret = false;
	if (node.u != -1 && node.v != -1) {
		int idx0 = node.v * mWidth + node.u;
		int idx1 = node.v * mWidth + node.u + 1;
		int idx2 = (node.v + 1) * mWidth + node.u;
		int idx3 = (node.v + 1) * mWidth + node.u + 1;

		if (intersectTriangle(ray, idx0, idx1, idx2, intersection)) {
			ret = true;
		} else if (intersectTriangle(ray, idx3, idx2, idx1, intersection)) {
			ret = true;
		}
	}
	else {
		for (const std::unique_ptr<BvhNode> &child : node.children) {
			float volumeDistance;
			if (child->volume.intersectRay(rayData, volumeDistance) && volumeDistance < intersection.distance) {
				if (intersectBvhNode(ray, rayData, *child, intersection)) {
					ret = true;
				}
			}
		}
	}

	return ret;
}

bool Grid::sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const
{
	return false;
}

}
}