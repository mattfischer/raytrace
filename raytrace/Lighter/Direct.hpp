#ifndef LIGHTER_DIRECT_HPP
#define LIGHTER_DIRECT_HPP

#include "Lighter/Base.hpp"

#include <random>

namespace Lighter {

class Direct : public Base
{
public:
	Direct(int numSamples);
	virtual Object::Radiance light(const Object::Intersection &intersection, Render::Tracer &tracer, Probe *probe = 0) const;

private:
	int mNumSamples;
	mutable std::default_random_engine mRandomEngine;
};

}

#endif