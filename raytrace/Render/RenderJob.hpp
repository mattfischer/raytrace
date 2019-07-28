#ifndef RENDER_RENDER_JOB_HPP
#define RENDER_RENDER_JOB_HPP

#include "Render/TileJob.hpp"
#include "Render/Settings.hpp"

#include "Object/Scene.hpp"
#include "Lighter/Master.hpp"

namespace Render {
	class RenderJob : public TileJob
	{
	public:
		RenderJob(const Object::Scene &scene, const Settings &settings, const Lighter::Master &lighter, Framebuffer &framebuffer);

	private:
		virtual void renderPixel(int x, int y, Sampler &sampler);

		const Object::Scene &mScene;
		const Settings &mSettings;
		const Lighter::Master &mLighter;
	};
}
#endif