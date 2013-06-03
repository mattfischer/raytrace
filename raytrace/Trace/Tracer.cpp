#include "Trace/Tracer.hpp"
#include "Object/Base.hpp"
#include "Object/Scene.hpp"
#include "Object/Camera.hpp"
#include "Object/Primitive/Base.hpp"

#include "Surface/Base.hpp"

#include "Lighter/Base.hpp"

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
}

void Tracer::popTrace()
{
	mIntersections.erase(mIntersections.begin() + mTraces.back(), mIntersections.end());
	mTraces.pop_back();
}

Object::Color Tracer::traceRay(const Trace::Ray &ray)
{
	IntersectionVector::iterator begin, end;
	intersect(ray, begin, end);
	std::sort(begin, end);

	Object::Color color(.2, .2, .2);

	if(begin != end)
	{
		color = begin->primitive()->surface()->color(*begin, *this);
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