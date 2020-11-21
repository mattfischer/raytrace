#include "Math/Vector2D.hpp"
#include "Math/Point2D.hpp"

namespace Math {
    Vector2D::Vector2D()
    {
        mU = mV = 0;
    }

    Vector2D::Vector2D(float u, float v)
    {
        mU = u;
        mV = v;
    }

    Vector2D::Vector2D(const Point2D &point)
        : mU(point.u()), mV(point.v())
    {
    }

    float Vector2D::u() const
    {
        return mU;
    }

    float Vector2D::v() const
    {
        return mV;
    }

    float Vector2D::magnitude2() const
    {
        return mU * mU + mV * mV;
    }

    Vector2D Vector2D::operator*(float b) const
    {
        return Vector2D(mU * b, mV * b);
    }

    Vector2D Vector2D::operator/(float b) const
    {
        return Vector2D(mU / b, mV / b);
    }

    Vector2D Vector2D::operator+(const Vector2D &b) const
    {
        return Vector2D(mU + b.u(), mV + b.v());
    }

    Vector2D Vector2D::operator-(const Vector2D &b) const
    {
        return Vector2D(mU - b.u(), mV - b.v());
    }

    float Vector2D::operator%(const Vector2D &b) const
    {
        return u() * b.v() - v() * b.u();
    }
}