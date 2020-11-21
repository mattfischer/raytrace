#include "Math/Bivector2D.hpp"

namespace Math {
    Bivector2D::Bivector2D(const Math::Vector2D &u, const Math::Vector2D &v)
        : mU(u), mV(v)
    {
    }

    const Math::Vector2D &Bivector2D::u() const
    {
        return mU;
    }

    const Math::Vector2D &Bivector2D::v() const
    {
        return mV;
    }

    Bivector2D Bivector2D::operator*(float b) const
    {
        return Bivector2D(mU * b, mV * b);
    }

    Bivector2D Bivector2D::operator+(const Bivector2D &b) const
    {
        return Bivector2D(mU + b.mU, mV + b.mV);
    }

    Bivector2D Bivector2D::operator-(const Bivector2D &b) const
    {
        return Bivector2D(mU - b.mU, mV - b.mV);
    }

    Bivector2D Bivector2D::operator/(float b) const
    {
        return Bivector2D(mU / b, mV / b);
    }

    Math::Vector2D Bivector2D::operator*(const Math::Vector2D &b) const
    {
        return mU * b.u() + mV * b.v();
    }
}