#ifndef RENDER_TRACER_HPP
#define RENDER_TRACER_HPP

#include "Object/Scene.hpp"
#include "Render/Sampler.hpp"

#include "Math/Ray.hpp"
#include "Math/Beam.hpp"

namespace Render {
	class Tracer
	{
	public:
		Tracer(const Object::Scene &scene, int width, int height, Sampler &sampler);

		const Object::Scene &scene() const;

		Sampler &sampler();

	protected:
		const Object::Scene &mScene;
		int mWidth;
		int mHeight;
		Sampler &mSampler;
	};
}

#endif
