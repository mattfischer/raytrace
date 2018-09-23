#include "Trace/Tracer.hpp"
#include "Object/Base.hpp"
#include "Object/Scene.hpp"
#include "Object/Camera.hpp"
#include "Object/Surface.hpp"

#include "Object/Brdf/Base.hpp"
#include "Object/Albedo/Base.hpp"
#include "Object/Primitive/Base.hpp"

#include <algorithm>

namespace Trace {

Tracer::Tracer(const Object::Scene &scene, const Settings &settings, RenderData &renderData, const std::vector<std::unique_ptr<Lighter::Base>> &lighters)
	: mScene(scene)
	, mRenderData(renderData)
	, mLighters(lighters)
{
	mSettings = settings;
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

Trace::Intersection Tracer::intersect(const Trace::Ray &ray)
{
	Trace::Intersection intersection;

	for (const std::unique_ptr<Object::Primitive::Base> &primitive : mScene.primitives())
	{
		if (primitive->boundingVolume().intersectRay(ray)) {
			Trace::Intersection newIntersection = primitive->intersect(ray);
			if (!intersection.valid() || newIntersection.distance() < intersection.distance()) {
				intersection = newIntersection;
			}
		}
	}

	return intersection;
}

Object::Radiance Tracer::traceRay(const Trace::Ray &ray)
{
	Intersection intersection = intersect(ray);

	Object::Radiance radiance;
	if(intersection.valid())
	{
		for (const std::unique_ptr<Lighter::Base> &lighter : mLighters) {
			radiance += lighter->light(intersection, *this);
		}
	}

	return radiance;
}

Object::Color Tracer::toneMap(const Object::Radiance &radiance)
{
	float red = radiance.red() / (radiance.red() + 1);
	float green = radiance.green() / (radiance.green() + 1);
	float blue = radiance.blue() / (radiance.blue() + 1);

	return Object::Color(red, green, blue);
}

Trace::Ray Tracer::createCameraRay(float x, float y)
{
	float cx = (2 * x - mSettings.width) / mSettings.width;
	float cy = (2 * y - mSettings.height) / mSettings.width;
	Trace::Ray ray = mScene.camera().createRay(cx, cy, 1);

	return ray;
}

Object::Color Tracer::tracePixel(float x, float y)
{
	Trace::Ray ray = createCameraRay(x, y);

	Object::Color color;
	if (mSettings.lighting)
	{
		Object::Radiance radiance = traceRay(ray);
		color = toneMap(radiance);
	}
	else
	{
		Intersection intersection =	intersect(ray);
		if (intersection.valid())
		{
			color = intersection.primitive()->surface().albedo().color(intersection.objectPoint());
		}
	}
	
	return color;
}

bool Tracer::prerenderPixel(float x, float y)
{
	bool ret = false;
	Trace::Ray ray = createCameraRay(x, y);

	Intersection intersection = intersect(ray);

	if (intersection.valid())
	{
		for (const std::unique_ptr<Lighter::Base> &lighter : mLighters) {
			ret |= lighter->prerender(intersection, *this);
		}
	}

	return ret;
}

float Tracer::projectedPixelSize(float distance)
{
	return mScene.camera().projectSize(2.0f / mSettings.width, distance);
}

}