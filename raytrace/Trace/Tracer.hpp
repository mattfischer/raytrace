#ifndef TRACE_H
#define TRACE_H

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
		int antialias;
		double hFOV;
		int maxRayGeneration;
	};

	Tracer(Object::Scene *scene, const Settings &settings);
	virtual ~Tracer();

	Object::Scene *scene() const;
	void setScene(Object::Scene *scene);

	Settings &settings();

	Object::Color tracePixel(int x, int y) const;

protected:
	Object::Color doLighting(const Math::Ray &ray, const Intersection &intersection) const;
	Object::Color traceRay(const Math::Ray &ray) const;

	Object::Scene *mScene;
	Settings mSettings;

	mutable std::vector<Intersection> mIntersections;
	mutable std::vector<Intersection> mLightIntersections;
};

}
#endif
