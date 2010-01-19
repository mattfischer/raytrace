#ifndef TRACE_H
#define TRACE_H

#include "color.h"
#include "scene.h"

#include "intersection.h"
#include <vector>

class Tracer
{
public:
	Tracer(Scene *scene = 0);
	virtual ~Tracer();

	Scene *scene() const;
	void setScene(Scene *scene);

	Color tracePixel(double x, double y, double width, double height) const;

protected:
	Color doLighting(const Ray &ray, const Intersection &intersection) const;

	Scene *mScene;

	mutable std::vector<Intersection> mIntersections;
	mutable std::vector<Intersection> mLightIntersections;
};

#endif
