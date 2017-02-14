#include "Trace/Tracer.hpp"
#include "Object/Base.hpp"
#include "Object/Scene.hpp"
#include "Object/Camera.hpp"
#include "Object/Surface.hpp"

#include "Object/Brdf/Base.hpp"
#include "Object/Albedo/Base.hpp"
#include "Object/Primitive/Base.hpp"

#include "Trace/Lighter/Direct.hpp"
#include "Trace/Lighter/Indirect.hpp"
#include "Trace/Lighter/Radiant.hpp"
#include "Trace/Lighter/Specular.hpp"

#include <algorithm>

namespace Trace {

Tracer::Tracer(const Object::Scene &scene, const Settings &settings, RenderData &renderData)
	: mScene(scene)
	, mRenderData(renderData)
{
	mSettings = settings;

	if (settings.directLighting) {
		mLighters.push_back(std::make_unique<Lighter::Direct>(settings.directSamples));
	}

	if (settings.indirectLighting) {
		mLighters.push_back(std::make_unique<Lighter::Indirect>(settings.indirectSamples, settings.indirectDirectSamples));
	}

	if (settings.radiantLighting) {
		mLighters.push_back(std::make_unique<Lighter::Radiant>());
	}

	if (settings.specularLighting) {
		mLighters.push_back(std::make_unique<Lighter::Specular>());
	}
}

const Object::Scene &Tracer::scene() const
{
	return mScene;
}

Tracer::Settings &Tracer::settings()
{
	return mSettings;
}

Tracer::RenderData &Tracer::renderData()
{
	return mRenderData;
}

void Tracer::intersect(const Trace::Ray &ray, IntersectionVector::iterator &begin, IntersectionVector::iterator &end)
{
	mIntersections.clear();
	mScene.intersect(ray, mIntersections);

	begin = mIntersections.begin();
	end = mIntersections.end();
	std::sort(begin, end);
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

	return radiance;
}

float toneMap(float radiance)
{
	float color = radiance / (radiance + 1);

	return color;
}

Object::Color Tracer::tracePixel(float x, float y)
{
	float cx = (2 * x - mSettings.width) / mSettings.width;
	float cy = (2 * y - mSettings.height) / mSettings.width;
	Trace::Ray ray = mScene.camera().createRay(cx, cy, 1);

	Object::Color color;
	if (mSettings.lighting)
	{
		Object::Radiance radiance = traceRay(ray);
		color = Object::Color(toneMap(radiance.red()), toneMap(radiance.green()), toneMap(radiance.blue()));
	}
	else
	{
		IntersectionVector::iterator begin, end;

		intersect(ray, begin, end);

		if (begin != end)
		{
			const Intersection &intersection = *begin;
			color = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
		}
	}
	
	return color;
}

bool Tracer::prerenderPixel(float x, float y)
{
	bool ret = false;
	float cx = (2 * x - mSettings.width) / mSettings.width;
	float cy = (2 * y - mSettings.height) / mSettings.width;
	Trace::Ray ray = mScene.camera().createRay(cx, cy, 1);

	IntersectionVector::iterator begin, end;
	intersect(ray, begin, end);

	if (begin != end)
	{
		Intersection intersection = *begin;
		for (const std::unique_ptr<Lighter::Base> &lighter : mLighters) {
			ret |= lighter->prerender(intersection, *this);
		}
	}

	return ret;
}

}