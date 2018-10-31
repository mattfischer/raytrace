#ifndef RENDER_TRACER_HPP
#define RENDER_TRACER_HPP

#include "Object/Scene.hpp"
#include "Render/Intersection.hpp"

#include "Math/Ray.hpp"

namespace Render {
	class Tracer
	{
	public:
		Tracer(const Object::Scene &scene, int width, int height);

		const Object::Scene &scene() const;

		Intersection intersect(const Math::Ray &ray);

		Math::Ray createCameraRay(float x, float y);

		float projectedPixelSize(float distance);

	protected:
		const Object::Scene &mScene;
		int mWidth;
		int mHeight;
	};
}

#endif
