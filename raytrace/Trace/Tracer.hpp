#ifndef TRACE_TRACER_HPP
#define TRACE_TRACER_HPP

#include "Object/Color.hpp"
#include "Object/Scene.hpp"

#include "Trace/Intersection.hpp"
#include <vector>

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
	void setScene(Object::Scene *scene);

	Settings &settings();
	const Settings &settings() const;

	Object::Color tracePixel(float x, float y) const;
	Object::Color traceRay(const Trace::Ray &ray) const;

	IntersectionVector &intersections() const;

protected:
	Object::Scene *mScene;
	Settings mSettings;

	mutable IntersectionVector mIntersections;
};

}
#endif
