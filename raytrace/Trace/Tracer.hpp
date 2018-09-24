#ifndef TRACE_TRACER_HPP
#define TRACE_TRACER_HPP

#include "Trace/Forwards.hpp"
#include "Object/Forwards.hpp"

#include "Trace/Intersection.hpp"

namespace Trace {

class Tracer
{
public:
	Tracer(const Object::Scene &scene, int width, int height);

	const Object::Scene &scene() const;

	Trace::Intersection intersect(const Trace::Ray &ray);

	Trace::Ray createCameraRay(float x, float y);

	float projectedPixelSize(float distance);

protected:
	const Object::Scene &mScene;
	int mWidth;
	int mHeight;
};

}
#endif
