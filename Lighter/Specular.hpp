#ifndef LIGHTER_SPECULAR_HPP
#define LIGHTER_SPECULAR_HPP

#include "Lighter/Base.hpp"
#include "Render/Engine.hpp"

namespace Lighter {
	class Specular : public Base
	{
	public:
		Specular(const Lighter::Base &lighter, int maxGeneration, bool misDirect);

		virtual Object::Radiance light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;

	private:
		const Lighter::Base &mLighter;
		int mMaxGeneration;
		bool mMisDirect;
	};
}

#endif