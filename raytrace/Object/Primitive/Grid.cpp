#include "Object/Primitive/Grid.hpp"

namespace Object {
namespace Primitive {

Grid::Grid(int width, int height, std::vector<Math::Point> &&points)
	: mWidth(width), mHeight(height), mPoints(std::move(points))
{
	computeBoundingVolume();
}

bool intersectTriangle(const Math::Ray &ray, const Math::Point &point0, const Math::Point &point1, const Math::Point &point2, float &distance)
{
	Math::Vector E1 = point1 - point0;
	Math::Vector E2 = point2 - point0;
	Math::Vector P = ray.direction() % E2;

	float den = P * E1;
	if (std::abs(den) < 0.001) {
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

	return (distance >= 0);
}

Intersection Grid::doIntersect(const Math::Ray &ray) const
{
	Intersection intersection;

	for (int i = 0; i < mHeight - 1; i++) {
		for (int j = 0; j < mWidth - 1; j++) {
			float distance;

			const Math::Point point0 = mPoints[i * mWidth + j];
			const Math::Point point1 = mPoints[i * mWidth + j + 1];
			const Math::Point point2 = mPoints[(i + 1) * mWidth + j];
			const Math::Point point3 = mPoints[(i + 1) * mWidth + j + 1];

			if (intersectTriangle(ray, point0, point1, point2, distance)) {
				if (!intersection.valid() || distance < intersection.distance()) {
					Math::Normal normal = Math::Normal((point1 - point0) % (point2 - point0)).normalize();
					intersection = Intersection(this, ray, distance, normal, ray.origin() + ray.direction() * distance);
				}
			}

			if (intersectTriangle(ray, point3, point2, point1, distance)) {
				if (!intersection.valid() || distance < intersection.distance()) {
					Math::Normal normal = Math::Normal((point2 - point3) % (point1 - point3)).normalize();
					intersection = Intersection(this, ray, distance, normal, ray.origin() + ray.direction() * distance);
				}
			}
		}
	}

	return intersection;
}

bool Grid::doInside(const Math::Point &point) const
{
	return false;
}

BoundingVolume Grid::doBoundingVolume() const
{
	BoundingVolume volume;
	for (const Math::Point &point : mPoints) {
		volume.expand(transformation() * point);
	}

	return volume;
}

}
}