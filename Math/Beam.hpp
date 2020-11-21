#ifndef MATH_BEAM_HPP
#define MATH_BEAM_HPP

#include "Math/Ray.hpp"
#include "Math/Bivector.hpp"

namespace Math {
    class Beam
    {
    public:
        Beam() = default;
        Beam(const Math::Ray &ray, const Math::Bivector &originDifferential, const Math::Bivector &directionDifferential);

        const Math::Ray &ray() const;
        const Math::Bivector &originDifferential() const;
        const Math::Bivector &directionDifferential() const;

        Math::Bivector project(float distance, const Math::Normal &normal) const;

    private:
        Math::Ray mRay;
        Math::Bivector mOriginDifferential;
        Math::Bivector mDirectionDifferential;
    };
}
#endif