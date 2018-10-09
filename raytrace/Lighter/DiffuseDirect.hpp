#ifndef LIGHTER_DIFFUSE_DIRECT_HPP
#define LIGHTER_DIFFUSE_DIRECT_HPP

#include "Lighter/Base.hpp"

#include <random>

namespace Lighter {

class DiffuseDirect : public Base
{
public:
	DiffuseDirect(int numSamples);
	virtual Object::Radiance light(const Object::Intersection &intersection, Render::Tracer &tracer) const;

private:
	int mNumSamples;
	mutable std::default_random_engine mRandomEngine;
};

}

#endif