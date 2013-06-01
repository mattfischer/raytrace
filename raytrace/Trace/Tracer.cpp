#include "Trace/Tracer.hpp"
#include "Object/Base.hpp"

#include <algorithm>

#include <math.h>

namespace Trace {

Tracer::Tracer(Object::Scene *scene, const Settings &settings)
{
	mScene = scene;
	mSettings = settings;
}

Tracer::~Tracer()
{
}

Object::Scene *Tracer::scene() const
{
	return mScene;
}

Tracer::Settings &Tracer::settings()
{
	return mSettings;
}

const Tracer::Settings &Tracer::settings() const
{
	return mSettings;
}

void Tracer::setScene(Object::Scene *scene)
{
	mScene = scene;
}

IntersectionVector &Tracer::intersections() const
{
	return mIntersections;
}

Object::Color Tracer::traceRay(const Trace::Ray &ray) const
{
	int startSize = mIntersections.size();
	mScene->findIntersections(ray, mIntersections);

	Object::Color color(.2, .2, .2);

	if(mIntersections.size() > startSize)
	{
		const Intersection &intersection = mIntersections[startSize];
		color = intersection.primitive()->surface()->color(intersection, *this);
	}
	mIntersections.erase(mIntersections.begin() + startSize, mIntersections.end());

	return color.clamp();
}

Object::Color Tracer::tracePixel(float x, float y) const
{
	float aspectRatio = (float)mSettings.height / (float)mSettings.width;

	Trace::Ray ray = mScene->camera()->createRay(x / mSettings.width, y / mSettings.height, aspectRatio, 1);
	return traceRay(ray);
}

}