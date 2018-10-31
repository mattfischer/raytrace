#ifndef LIGHTER_SPECULAR_HPP
#define LIGHTER_SPECULAR_HPP

#include "Lighter/Base.hpp"
#include "Render/Engine.hpp"

#include <random>

namespace Lighter {
	class Specular : public Base
	{
	public:
		Specular(const Lighter::Base &lighter, int numSamples, int maxGeneration, bool misDirect, int numDirectSamples);

		virtual Object::Radiance light(const Render::Intersection &intersection, Render::Tracer &tracer, int generation) const;

	private:
		const Lighter::Base &mLighter;
		int mNumSamples;
		int mMaxGeneration;
		bool mMisDirect;
		int mNumDirectSamples;
		mutable std::default_random_engine mRandomEngine;
	};
}

#endif