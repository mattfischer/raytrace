#ifndef LIGHTER_TRANSMIT_HPP
#define LIGHTER_TRANSMIT_HPP

#include "Lighter/Base.hpp"

#include <random>

namespace Lighter {
	class Transmit : public Base
	{
	public:
		Transmit(const Lighter::Base &lighter, int maxGeneration);

		virtual Object::Radiance light(const Object::Intersection &intersection, Render::Tracer &tracer, int generation) const;

	private:
		const Lighter::Base &mLighter;
		int mMaxGeneration;
		mutable std::default_random_engine mRandomEngine;
	};
}

#endif