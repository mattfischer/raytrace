#ifndef LIGHTER_SPECULAR_HPP
#define LIGHTER_SPECULAR_HPP

#include "Lighter/Base.hpp"
#include "Render/Engine.hpp"

namespace Lighter {

	class Specular : public Base
	{
	public:
		Specular(const Render::Engine &engine, int maxRayGeneration);

		virtual Object::Radiance light(const Object::Intersection &intersection, Render::Tracer &tracer, Probe *probe = 0) const;

	private:
		const Render::Engine &mEngine;
		int mMaxRayGeneration;
	};

}

#endif