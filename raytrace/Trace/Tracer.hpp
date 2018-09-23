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
		int antialiasSamples;
		bool radiantLighting;
		bool specularLighting;
		bool directLighting;
		int directSamples;
		bool indirectLighting;
		int indirectSamples;
		int indirectDirectSamples;
		bool irradianceCaching;
		float irradianceCacheThreshold;
	};

	Tracer(const Object::Scene &scene, const Settings &settings, const std::vector<std::unique_ptr<Lighter::Base>> &lighters);

	const Object::Scene &scene() const;
	Settings &settings();

	Trace::Intersection intersect(const Trace::Ray &ray);

	Trace::Ray createCameraRay(float x, float y);
	Object::Color tracePixel(float x, float y);
	Object::Radiance traceRay(const Trace::Ray &ray);

	bool prerenderPixel(float x, float y);

	Object::Color toneMap(const Object::Radiance &radiance);
	float projectedPixelSize(float distance);

protected:
	const Object::Scene &mScene;
	Settings mSettings;

	const std::vector<std::unique_ptr<Lighter::Base>> &mLighters;
};

}
#endif
