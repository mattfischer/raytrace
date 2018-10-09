#ifndef LIGHTER_SPECULAR_HPP
#define LIGHTER_SPECULAR_HPP

#include "Lighter/Base.hpp"
#include "Render/Engine.hpp"

namespace Lighter {

	class Specular : public Base
	{
	public:
		Specular(const Lighter::Base &lighter, int maxRayGeneration);

		virtual Object::Radiance light(const Object::Intersection &intersection, Render::Tracer &tracer) const;

	private:
		const Lighter::Base &mLighter;
		int mMaxRayGeneration;
	};

}

#endif