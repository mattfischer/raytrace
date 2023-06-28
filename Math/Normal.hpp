#ifndef MATH_NORMAL_HPP
#define MATH_NORMAL_HPP

#include "Proxies.hpp"

namespace Math {
    class Point;
    class BaseTransformation;
    class Vector;
    class Matrix;
    class Normal
    {
    public:
        Normal();
        Normal(float x, float y, float z);
        explicit Normal(const Point &point);
        explicit Normal(const Vector &vector);
        explicit Normal(const NormalProxy &proxy);

        float x() const;
        float y() const;
        float z() const;

        Normal normalize() const;
        float magnitude() const;
        float magnitude2() const;

        Normal operator+(const Normal &b) const;
        Normal operator-(const Normal &b) const;
        float operator*(const Normal &b) const;
        float operator*(const Vector &b) const;
        Normal operator*(float b) const;
        Normal operator/(float b) const;
        Normal operator-() const;
        Normal operator%(const Normal &b) const;

        void writeProxy(NormalProxy &proxy) const;

    private:
        float mX;
        float mY;
        float mZ;
    };

    Normal operator*(const BaseTransformation &transformation, const Normal &normal);
    Normal operator*(const Normal &normal, const Matrix &matrix);
    float operator*(const Vector &vector, const Normal &normal);
}

#endif
