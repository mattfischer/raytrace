#include "Math/Normal.hpp"
#include "Math/Point.hpp"
#include "Math/Transformation.hpp"
#include "Math/Vector.hpp"

#include <cmath>

namespace Math {

    Normal::Normal()
    {
        mX = mY = mZ = 0;
    }

    Normal::Normal(float x, float y, float z)
    {
        mX = x;
        mY = y;
        mZ = z;
    }

    Normal::Normal(const Vector &vector)
    {
        mX = vector.x();
        mY = vector.y();
        mZ = vector.z();
    }

    Normal::Normal(const Point &point)
    {
        mX = point.x();
        mY = point.y();
        mZ = point.z();
    }

    float Normal::x() const
    {
        return mX;
    }

    float Normal::y() const
    {
        return mY;
    }

    float Normal::z() const
    {
        return mZ;
    }

    float Normal::magnitude() const
    {
        return std::sqrt(magnitude2());
    }

    float Normal::magnitude2() const
    {
        return *this * *this;
    }

    Normal Normal::normalize() const
    {
        float m = magnitude();

        return Normal(x() / m, y() / m, z() / m);
    }

    Normal Normal::operator+(const Normal &b) const
    {
        return Normal(x() + b.x(), y() + b.y(), z() + b.z());
    }

    Normal Normal::operator-(const Normal &b) const
    {
        return Normal(x() - b.x(), y() - b.y(), z() - b.z());
    }

    float Normal::operator*(const Normal &b) const
    {
        return x() * b.x() + y() * b.y() + z() * b.z();
    }

    Normal operator*(const BaseTransformation &transformation, const Normal &normal)
    {
        return Normal(normal * transformation.inverseMatrix());
    }

    Normal Normal::operator-() const
    {
        return Normal(-x(), -y(), -z());
    }

    float Normal::operator*(const Vector &b) const
    {
        return x() * b.x() + y() * b.y() + z() * b.z();
    }

    Normal Normal::operator*(float b) const
    {
        return Normal(x() * b, y() * b, z() * b);
    }

    Normal Normal::operator/(float b) const
    {
        return Normal(x() / b, y() / b, z() / b);
    }

    Normal Normal::operator%(const Math::Normal &b) const
    {
        return Normal(y() * b.z() - z() * b.y(), z() * b.x() - x() * b.z(), x() * b.y() - y() * b.x());
    }

    float operator*(const Vector &vector, const Normal &normal)
    {
        return normal * vector;
    }

    Normal operator*(const Normal &normal, const Matrix &matrix)
    {
        if (matrix.identity()) return normal;

        float x = matrix(0, 0) * normal.x() + matrix(0, 1) * normal.y() + matrix(0, 2) * normal.z();
        float y = matrix(1, 0) * normal.x() + matrix(1, 1) * normal.y() + matrix(1, 2) * normal.z();
        float z = matrix(2, 0) * normal.x() + matrix(2, 1) * normal.y() + matrix(2, 2) * normal.z();

        return Normal(x, y, z);
    }

}