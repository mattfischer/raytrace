#include "Math/Beam.hpp"

namespace Math {
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

    Math::Bivector Beam::project(float distance, const Math::Normal &normal) const
    {
        const Math::Bivector &dP = mOriginDifferential;
        const Math::Bivector &dD = mDirectionDifferential;
        const Math::Vector &D = mRay.direction();
        const Math::Vector N(normal);
        float t = distance;

        Math::Bivector A = dP + dD * t;
        Math::Bivector dtD = Math::Bivector(D * (A.u() * N), D * (A.v() * N)) / (N * D);

        return A - dtD;
    }
}