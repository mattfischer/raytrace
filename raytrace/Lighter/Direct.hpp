#ifndef LIGHTER_DIRECT_HPP
#define LIGHTER_DIRECT_HPP

#include "Lighter/Base.hpp"

#include <random>

namespace Lighter {
	class Direct : public Base
	{
	public:
		Direct(int numSamples, bool misSpecular, int numSpecularSamples);
		virtual Object::Radiance light(const Render::Intersection &intersection, Render::Tracer &tracer, int generation) const;

	private:
		int mNumSamples;
		bool mIncludeSpecular;
		bool mMisSpecular;
		int mNumSpecularSamples;
		mutable std::default_random_engine mRandomEngine;
	};
}

#endif