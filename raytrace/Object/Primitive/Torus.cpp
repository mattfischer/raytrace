#include "Object/Primitive/Torus.hpp"
#include "Math/PolySolve.hpp"

#include <math.h>

namespace Object {
namespace Primitive {

Torus::Torus()
{
}

bool cylIntersect(const Trace::Ray &ray, float radius, float results[])
{
	float a, b, c;
	float disc;

	a = ray.direction().x() * ray.direction().x() + ray.direction().y() * ray.direction().y();
	b = 2 * (ray.origin().x() * ray.direction().x() + ray.origin().y() * ray.direction().y());
	c = ray.origin().x() * ray.origin().x() + ray.origin().y() * ray.origin().y() - radius * radius;

	disc = b * b - 4 * a * c;

	if(disc >= 0)
	{
		results[0] = (-b - sqrt(disc)) / (2 * a);
		results[1] = (-b + sqrt(disc)) / (2 * a);
		return true;
	}

	return false;
}

void Torus::doIntersect(const Trace::Ray &ray, std::vector<Trace::Intersection> &intersections) const
{
	float r = 0.25;
	float r2 = r * r;

	float ox = ray.origin().x();
	float oy = ray.origin().y();
	float oz = ray.origin().z();

	float dx = ray.direction().x();
	float dy = ray.direction().y();
	float dz = ray.direction().z();

	float ox2 = ox * ox, oy2 = oy * oy, oz2 = oz * oz;
	float dx2 = dx * dx, dy2 = dy * dy, dz2 = dz * dz;

	float ox3 = ox2 * ox, oy3 = oy2 * oy, oz3 = oz2 * oz;
	float dx3 = dx2 * dx, dy3 = dy2 * dy, dz3 = dz2 * dz;

	float ox4 = ox3 * ox, oy4 = oy3 * oy, oz4 = oz3 * oz;
	float dx4 = dx3 * dx, dy4 = dy3 * dy, dz4 = dz3 * dz;

	float c[5];
	c[4] = dx + dy + dz + 2 * ( dx2 * dy2 + dy2 * dz2 + dx2 * dz2 );
	c[3] = 4 * ( ox * dx3 + oy * dy3 + oz * dz3 + 
		     dx2 * oy * dy + dy2 * oz * dz + dx2 * oz * dz +
		     ox * dx * dy2 + oy * dy * dz2 + ox * dx * dz2 );
	c[2] = 6 * ( ox2 * dx2 + oy2 * dy2 + oz2 * dz2 ) +
	       2 * ( ox2 * dy2 + oy2 * dz2 + ox2 * dz2 + oy2 * dx2 + oz2 * dy2 + oz2 * dx2 ) +
	       8 * ( ox * dx * oy * dy + oy * dy * oz * dz + ox * dx * oz * dz ) +
	      -2 * ( dx2 * ( r2 + 1 ) + dy * ( r2 + 1 ) + dz * ( r2 - 1 ) );
	c[1] = 4 * ( ox3 * dx + oy3 * dy + oz3 * dz + 
		     ox2 * oy * dy + oy2 * oz * dz + ox2 * oz * dz + 
		     ox * oy2 * dy + oy * oz2 * dz + ox * oz2 * dz +
		     -1 * ( ox * dx * ( r2 + 1 ) + oy * dy * ( r2 + 1 ) + oz * dz * ( r2 - 1 ) ));
	c[0] = ox4 + oy4 + oz4 + 
	        2 * ( ox2 * oy2 + oy2 * oz2 + ox2 * oz2 ) +
	       -2 * ( ox2 * ( r2 + 1 ) + oy2 * ( r2 + 1 ) + oz2 * ( r2 - 1 ) ) +
		      r2 * r2 + 1;

	float guesses[4];
	float results[4];
	if(!cylIntersect(ray, 1 + r, guesses))
	{
		return;
	}
	if(!cylIntersect(ray, 1 - r, guesses + 2))
	{
		cylIntersect(ray, 1 + r, guesses + 2);
	}

	Math::PolySolve::newtonRhapson(c, 5, guesses, .1, results);
	for(int i=0; i<4; i++)
	{
		if(results[i] == HUGE_VAL) continue;
		if(results[i] < 0.01) continue;

		Math::Point point = ray.origin() + ray.direction() * results[i];
		
		intersections.push_back(Trace::Intersection(this, results[i], Math::Normal(point), point));
	}
}

bool Torus::doInside(const Math::Point &point) const
{
	return false;
}

}
}