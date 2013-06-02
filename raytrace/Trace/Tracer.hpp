#ifndef TRACE_TRACER_HPP
#define TRACE_TRACER_HPP

#include "Trace/Forwards.hpp"
#include "Object/Forwards.hpp"

#include "Object/Color.hpp"
#include "Trace/Intersection.hpp"

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
	};

	Tracer(Object::Scene *scene, const Settings &settings);
	virtual ~Tracer();

	Object::Scene *scene() const;

	Settings &settings();

	Object::Color tracePixel(float x, float y);
	Object::Color traceRay(const Trace::Ray &ray);

	IntersectionVector &intersections();

protected:
	Object::Scene *mScene;
	Settings mSettings;

	IntersectionVector mIntersections;
};

}
#endif
