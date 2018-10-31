#include "Render/Beam.hpp"

namespace Render {
	Beam::Beam(const Math::Ray &ray, const Math::Bivector &originDifferential, const Math::Bivector &directionDifferential)
		: mRay(ray), mOriginDifferential(originDifferential), mDirectionDifferential(directionDifferential)
	{
	}

	const Math::Ray &Beam::ray() const
	{
		return mRay;
	}

	const Math::Bivector &Beam::originDifferential() const
	{
		return mOriginDifferential;
	}

	const Math::Bivector &Beam::directionDifferential() const
	{
		return mDirectionDifferential;
	}
}