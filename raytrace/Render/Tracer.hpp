#ifndef RENDER_TRACER_HPP
#define RENDER_TRACER_HPP

#include "Trace/Forwards.hpp"
#include "Object/Forwards.hpp"

#include "Object/Intersection.hpp"

namespace Render {

class Tracer
{
public:
	Tracer(const Object::Scene &scene, int width, int height);

	const Object::Scene &scene() const;

	Object::Intersection intersect(const Math::Ray &ray);

	Math::Ray createCameraRay(float x, float y);

	float projectedPixelSize(float distance);

protected:
	const Object::Scene &mScene;
	int mWidth;
	int mHeight;
};

}
#endif
