#ifndef RENDER_BEAM_HPP
#define RENDER_BEAM_HPP

#include "Math/Ray.hpp"
#include "Math/Bivector.hpp"

namespace Render {
	class Beam
	{
	public:
		Beam() = default;
		Beam(const Math::Ray &ray, const Math::Bivector &originDifferential, const Math::Bivector &directionDifferential);

		const Math::Ray &ray() const;
		const Math::Bivector &originDifferential() const;
		const Math::Bivector &directionDifferential() const;

	private:
		Math::Ray mRay;
		Math::Bivector mOriginDifferential;
		Math::Bivector mDirectionDifferential;
	};
}
#endif