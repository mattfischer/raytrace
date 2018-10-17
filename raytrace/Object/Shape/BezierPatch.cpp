#include "Object/Shape/BezierPatch.hpp"

namespace Object {
namespace Shape {

BezierPatch::BezierPatch(int width, int height, std::vector<Math::Point> &&controlPoints)
{
	std::vector<Math::Point> points;
	std::vector<Math::Normal> normals;

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			float s = float(i) / (width - 1);
			float t = float(j) / (height - 1);

			float Bs[4] = { (1 - s)*(1 - s)*(1 - s), 3 * s*(1 - s)*(1 - s), 3 * s*s*(1 - s), s*s*s };
			float Bt[4] = { (1 - t)*(1 - t)*(1 - t), 3 * t*(1 - t)*(1 - t), 3 * t*t*(1 - t), t*t*t };
			float Bds[4] = { -3 * (1 - s)*(1 - s), 3 * (1 - s)*(1 - s) - 3 * 2 * s * (1 - s), 3 * 2 * s*(1 - s) - 3 * s * s, 3 * s*s };
			float Bdt[4] = { -3 * (1 - t)*(1 - t), 3 * (1 - t)*(1 - t) - 3 * 2 * t * (1 - t), 3 * 2 * t*(1 - t) - 3 * t * t, 3 * t*t };

			Math::Vector p;
			Math::Vector ds;
			Math::Vector dt;
			for (int k = 0; k < 4; k++) {
				for (int l = 0; l < 4; l++) {
					p = p + Math::Vector(controlPoints[k * 4 + l]) * (Bs[l] * Bt[k]);
					ds = ds + Math::Vector(controlPoints[k * 4 + l]) * (Bds[l] * Bt[k]);
					dt = dt + Math::Vector(controlPoints[k * 4 + l]) * (Bs[l] * Bdt[k]);
				}
			}

			points.push_back(Math::Point(p));
			normals.push_back(Math::Normal(ds % dt).normalize());
		}
	}

	mGrid = std::make_unique<Grid>(width, height, std::move(points), std::move(normals));
}

bool BezierPatch::intersect(const Math::Ray &ray, Intersection &intersection) const
{
	return mGrid->intersect(ray, intersection);
}

BoundingVolume BezierPatch::boundingVolume(const Math::Transformation &transformation) const
{
	return mGrid->boundingVolume(transformation);
}

bool BezierPatch::sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const
{
	return mGrid->sample(u, v, point, du, dv, normal);
}

}
}