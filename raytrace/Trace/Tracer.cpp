#include "Trace/Tracer.hpp"
#include "Object/Base.hpp"
#include "Object/Scene.hpp"
#include "Object/Camera.hpp"
#include "Object/Surface.hpp"

#include "Object/Brdf/Base.hpp"
#include "Object/Albedo/Base.hpp"
#include "Object/Primitive/Base.hpp"

#include "Trace/Lighter/Base.hpp"

#include <algorithm>

namespace Trace {

Tracer::Tracer(const Object::Scene &scene, const Settings &settings)
	: mScene(scene)
{
	mSettings = settings;
	mLighters = Lighter::Base::createLighters();
	mGeneration = 0;
}

const Object::Scene &Tracer::scene() const
{
	return mScene;
}

Tracer::Settings &Tracer::settings()
{
	return mSettings;
}

int Tracer::generation() const
{
	return mGeneration;
}

void Tracer::intersect(const Trace::Ray &ray, IntersectionVector::iterator &begin, IntersectionVector::iterator &end)
{
	mGeneration++;
	mTraces.push_back(mIntersections.size());
	mScene.intersect(ray, mIntersections);

	begin = mIntersections.begin() + mTraces.back();
	end = mIntersections.end();
	std::sort(begin, end);
}

void Tracer::popTrace()
{
	mIntersections.erase(mIntersections.begin() + mTraces.back(), mIntersections.end());
	mTraces.pop_back();
	mGeneration--;
}

Object::Radiance Tracer::traceRay(const Trace::Ray &ray)
{
	IntersectionVector::iterator begin, end;

	intersect(ray, begin, end);

	Object::Radiance radiance;
	if(begin != end)
	{
		Intersection intersection = *begin;
		for (const std::unique_ptr<Lighter::Base> &lighter : mLighters) {
			radiance += lighter->light(intersection, *this);
		}
	}

	popTrace();

	return radiance;
}

Object::Color Tracer::tracePixel(float x, float y)
{
	float cx = (2 * x - mSettings.width) / mSettings.width;
	float cy = (2 * y - mSettings.height) / mSettings.width;
	Trace::Ray ray = mScene.camera().createRay(cx, cy);
	Object::Radiance radiance = traceRay(ray);
	Object::Color color(radiance.red(), radiance.green(), radiance.blue());

	return color.clamp();
}

}