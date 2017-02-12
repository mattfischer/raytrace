#ifndef TRACE_TRACER_HPP
#define TRACE_TRACER_HPP

#include "Trace/Forwards.hpp"
#include "Object/Forwards.hpp"

#include "Object/Color.hpp"
#include "Trace/Intersection.hpp"
#include "Trace/Lighter/Base.hpp"

#include <vector>
#include <memory>

namespace Trace {

class Tracer
{
public:
	struct Settings
	{
		int width;
		int height;
		bool lighting;
		int maxRayGeneration;
		float threshold;
		int maxAAGen;
		bool radiantLighting;
		bool specularLighting;
		bool directLighting;
		int directSamples;
		bool indirectLighting;
		int indirectSamples;
		int indirectDirectSamples;
	};

	Tracer(const Object::Scene &scene, const Settings &settings);

	const Object::Scene &scene() const;
	Settings &settings();

	void intersect(const Trace::Ray &ray, IntersectionVector::iterator &begin, IntersectionVector::iterator &end);

	Object::Color tracePixel(float x, float y);
	Object::Radiance traceRay(const Trace::Ray &ray);

protected:
	const Object::Scene &mScene;
	Settings mSettings;

	std::vector<std::unique_ptr<Lighter::Base>> mLighters;
	IntersectionVector mIntersections;
};

}
#endif
