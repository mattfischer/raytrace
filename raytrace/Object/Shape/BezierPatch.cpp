#include "Object/Shape/BezierPatch.hpp"

namespace Object {
namespace Shape {

BezierPatch::BezierPatch(std::vector<Math::Point> &&controlPoints)
	: mControlPoints(std::move(controlPoints))
{
}

std::unique_ptr<Grid> BezierPatch::tesselate(int width, int height) const
{
	std::vector<Math::Point> points;

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			float s = float(i) / (width - 1);
			float t = float(j) / (height - 1);

			float Bs[4] = { (1 - s)*(1 - s)*(1 - s), 3 * s*(1 - s)*(1 - s), 3 * s*s*(1 - s), s*s*s };
			float Bt[4] = { (1 - t)*(1 - t)*(1 - t), 3 * t*(1 - t)*(1 - t), 3 * t*t*(1 - t), t*t*t };
			Math::Vector p;
			for (int k = 0; k < 4; k++) {
				for (int l = 0; l < 4; l++) {
					p = p + Math::Vector(mControlPoints[k * 4 + l]) * (Bs[l] * Bt[k]);
				}
			}
			points.push_back(Math::Point(p));
		}
	}

	return std::make_unique<Grid>(width, height, std::move(points));
}

}
}