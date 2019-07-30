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

		virtual std::unique_ptr<Job::ThreadLocal> createThreadLocal();

	private:
		struct ThreadLocal : public Job::ThreadLocal
		{
			Sampler sampler;

			ThreadLocal(int samplerDimensions) : sampler(samplerDimensions) {}
		};

		virtual void renderPixel(int x, int y, Job::ThreadLocal &threadLocal);
		virtual bool needRepeat();

		const Object::Scene &mScene;
		const Settings &mSettings;
		const Lighter::Master &mLighter;

		int mNumSamplesThisIteration;
		std::vector<bool> mPixelsDone;
		std::vector<Object::Radiance> mTotalRadiance;
		std::vector<unsigned int> mSamplerOffsets;
		int mNumSamplesCompleted;
		bool mNeedRepeat;
	};
}
#endif