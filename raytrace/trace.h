#ifndef TRACE_H
#define TRACE_H

#include "color.h"
#include "Object/Scene.hpp"

#include "intersection.h"
#include <vector>

class Tracer
{
public:
	struct Settings
	{
		int width;
		int height;
		bool lighting;
		int antialias;
		double hFOV;
		int maxRayGeneration;
	};

	Tracer(Object::Scene *scene, const Settings &settings);
	virtual ~Tracer();

	Object::Scene *scene() const;
	void setScene(Object::Scene *scene);

	Settings &settings();

	Color tracePixel(int x, int y) const;

protected:
	Color doLighting(const Math::Ray &ray, const Intersection &intersection) const;
	Color traceRay(const Math::Ray &ray) const;

	Object::Scene *mScene;
	Settings mSettings;

	mutable std::vector<Intersection> mIntersections;
	mutable std::vector<Intersection> mLightIntersections;
};

#endif
