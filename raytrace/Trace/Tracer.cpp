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

Object::Color Tracer::traceRay(const Trace::Ray &ray) const
{
	mIntersections.clear();
	mScene->findIntersections(ray, mIntersections);

	Object::Color color(.2, .2, .2);

	if(mIntersections.size() > 0)
	{
		color = mIntersections[0].primitive()->surface()->color(ray, mIntersections[0], *this);
	}
	return color.clamp();
}

Object::Color Tracer::tracePixel(float x, float y) const
{
	float aspectRatio = (float)mSettings.height / (float)mSettings.width;

	Trace::Ray ray = mScene->camera()->createRay(x / mSettings.width, y / mSettings.height, aspectRatio);
	return traceRay(ray);
}

}