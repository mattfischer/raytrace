#ifndef LIGHTER_DIRECT_HPP
#define LIGHTER_DIRECT_HPP

#include "Lighter/Base.hpp"

#include "Math/OrthonormalBasis.hpp"

namespace Lighter {
	class Direct : public Base
	{
	public:
		Direct(bool misSpecular);
		virtual Object::Radiance light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const;

		Object::Radiance sampleIrradiance(const Object::Intersection &intersection, const Math::OrthonormalBasis &basis, Render::Sampler &sampler, Math::Vector &localIncidentDirection) const;

	private:
		bool mMisSpecular;
	};
}

#endif