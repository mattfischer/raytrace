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
}

const Object::Scene &Tracer::scene() const
{
	return mScene;
}

Tracer::Settings &Tracer::settings()
{
	return mSettings;
}

void Tracer::intersect(const Trace::Ray &ray, IntersectionVector::iterator &begin, IntersectionVector::iterator &end)
{
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
}

class Accumulator : public Lighter::Base::Accumulator
{
public:
	Accumulator(const Math::Normal &normal, const Math::Vector &viewDirection, const Object::Color &albedo, const Object::Brdf::Base &brdf)
		: mNormal(normal), mViewDirection(viewDirection), mAlbedo(albedo), mBrdf(brdf)
	{
	}

	const Object::Radiance &totalRadiance()
	{
		return mTotalRadiance;
	}

	virtual void accumulate(const Object::Radiance &radiance, const Math::Vector &direction)
	{
		mTotalRadiance += mBrdf.radiance(radiance, direction, mNormal, mViewDirection, mAlbedo);
	}

private:
	const Math::Normal &mNormal;
	const Math::Vector &mViewDirection;
	const Object::Color &mAlbedo;
	const Object::Brdf::Base &mBrdf;
	Object::Radiance mTotalRadiance;
};

Object::Radiance Tracer::diffuseRadiance(const Intersection &intersection)
{
	const Object::Surface &surface = intersection.primitive()->surface();
	Math::Vector viewDirection = (mScene.camera().transformation().origin() - intersection.point()).normalize();
	Object::Color albedo = surface.albedo().color(intersection.objectPoint());

	Accumulator accumulator(intersection.normal(), viewDirection, albedo, surface.brdf());

	for(const std::unique_ptr<Lighter::Base> &lighter: mLighters) {
		lighter->light(intersection, *this, accumulator);
	}

	return accumulator.totalRadiance();
}

Object::Radiance Tracer::specularRadiance(const Intersection &intersection, int generation)
{
	const Object::Surface &surface = intersection.primitive()->surface();
	const Trace::Ray &ray = intersection.ray();
	Object::Radiance radiance;

	if (surface.brdf().specular() && generation < mSettings.maxRayGeneration) {
		Object::Color albedo = surface.albedo().color(intersection.objectPoint());
		Math::Vector incident = ray.direction();
		Math::Vector reflect = incident + Math::Vector(intersection.normal()) * (2 * (-intersection.normal() * incident));

		Trace::Ray reflectRay(intersection.point(), reflect);
		Object::Radiance reflectRadiance = traceRay(reflectRay, generation + 1);

		radiance += surface.brdf().specularRadiance(reflectRadiance, albedo);
	}

	return radiance;
}

Object::Radiance Tracer::traceRay(const Trace::Ray &ray, int generation)
{
	IntersectionVector::iterator begin, end;
	intersect(ray, begin, end);

	Object::Radiance radiance(.2, .2, .2);

	if(begin != end)
	{
		radiance = diffuseRadiance(*begin) + specularRadiance(*begin, generation);
	}
	popTrace();

	return radiance;
}

Object::Color Tracer::tracePixel(float x, float y)
{
	float cx = (2 * x - mSettings.width) / mSettings.width;
	float cy = (2 * y - mSettings.height) / mSettings.width;
	Trace::Ray ray = mScene.camera().createRay(cx, cy);
	Object::Radiance radiance = traceRay(ray, 1);
	Object::Color color(radiance.red(), radiance.green(), radiance.blue());

	return color.clamp();
}

}