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

Tracer::Tracer(Object::Scene *scene, const Settings &settings)
{
	mScene = scene;
	mSettings = settings;
	mLighters = Lighter::Base::createLighters();
}

Tracer::~Tracer()
{
	for(int i=0; i<mLighters.size(); i++) {
		delete mLighters[i];
	}
}

Object::Scene *Tracer::scene() const
{
	return mScene;
}

Tracer::Settings &Tracer::settings()
{
	return mSettings;
}

const Lighter::LighterVector &Tracer::lighters() const
{
	return mLighters;
}

void Tracer::intersect(const Trace::Ray &ray, IntersectionVector::iterator &begin, IntersectionVector::iterator &end)
{
	mTraces.push_back(mIntersections.size());
	mScene->intersect(ray, mIntersections);

	begin = mIntersections.begin() + mTraces.back();
	end = mIntersections.end();
	std::sort(begin, end);
}

void Tracer::popTrace()
{
	mIntersections.erase(mIntersections.begin() + mTraces.back(), mIntersections.end());
	mTraces.pop_back();
}

class Accumulator : public Lighter::Base::Accumulator
{
public:
	Accumulator(const Math::Normal &normal, const Math::Vector &viewDirection, const Object::Color &albedo, const Object::Brdf::Base *brdf)
		: mNormal(normal), mViewDirection(viewDirection), mAlbedo(albedo)
	{
		mBrdf = brdf;
	}

	const Object::Color &totalColor()
	{
		return mTotalColor;
	}

	virtual void accumulate(const Object::Color &color, const Math::Vector &direction)
	{
		mTotalColor += mBrdf->color(color, direction, mNormal, mViewDirection, mAlbedo);
	}

private:
	const Math::Normal &mNormal;
	const Math::Vector &mViewDirection;
	const Object::Color &mAlbedo;
	const Object::Brdf::Base *mBrdf;
	Object::Color mTotalColor;
};

Object::Color Tracer::diffuseColor(const Intersection &intersection)
{
	Object::Surface *surface = intersection.primitive()->surface();
	Math::Vector viewDirection = (mScene->camera()->transformation().origin() - intersection.point()).normalize();
	Object::Color albedo = surface->albedo()->color(intersection.objectPoint());

	Accumulator accumulator(intersection.normal(), viewDirection, albedo, surface->brdf());

	const Lighter::LighterVector &lighters = mLighters;
	for (int i = 0; i < lighters.size(); i++) {
		lighters[i]->light(intersection, *this, accumulator);
	}

	return accumulator.totalColor();
}

Object::Color Tracer::specularColor(const Intersection &intersection)
{
	Object::Surface *surface = intersection.primitive()->surface();
	const Trace::Ray &ray = intersection.ray();
	Object::Color color;

	if (surface->brdf()->specular() && ray.generation() < mSettings.maxRayGeneration) {
		Object::Color albedo = surface->albedo()->color(intersection.objectPoint());
		Math::Vector incident = ray.direction();
		Math::Vector reflect = incident + Math::Vector(intersection.normal()) * (2 * (-intersection.normal() * incident));

		Trace::Ray reflectRay(intersection.point(), reflect, ray.generation() + 1);
		Object::Color reflectColor = traceRay(reflectRay);

		color += surface->brdf()->specularColor(reflectColor, albedo);
	}

	return color;
}

Object::Color Tracer::traceRay(const Trace::Ray &ray)
{
	IntersectionVector::iterator begin, end;
	intersect(ray, begin, end);

	Object::Color color(.2, .2, .2);

	if(begin != end)
	{
		color = diffuseColor(*begin) + specularColor(*begin);
	}
	popTrace();

	return color.clamp();
}

Object::Color Tracer::tracePixel(float x, float y)
{
	float cx = (2 * x - mSettings.width) / mSettings.width;
	float cy = (2 * y - mSettings.height) / mSettings.width;
	Trace::Ray ray = mScene->camera()->createRay(cx, cy, 1);
	return traceRay(ray);
}

}