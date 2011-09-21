#ifndef TRACE_H
#define TRACE_H

#include "color.h"
#include "scene.h"

#include "intersection.h"
#include <vector>

class Tracer
{
public:
	struct Settings
	{
		int antialias;
		double hFOV;
		int maxRayGeneration;
	};

	Tracer(Scene *scene = 0);
	virtual ~Tracer();

	Scene *scene() const;
	void setScene(Scene *scene);

	Settings &settings();

	Color tracePixel(int x, int y, int width, int height) const;

protected:
	Color doLighting(const Ray &ray, const Intersection &intersection) const;
	Color traceRay(const Ray &ray) const;

	Scene *mScene;
	Settings mSettings;

	mutable std::vector<Intersection> mIntersections;
	mutable std::vector<Intersection> mLightIntersections;
};

#endif
