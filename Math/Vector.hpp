#ifndef MATH_VECTOR_HPP
#define MATH_VECTOR_HPP

#include "Proxies.hpp"

namespace Math {
    class Point;
    class Normal;
    class BaseTransformation;
    class Matrix;
    class Vector
    {
    public:
        Vector();
        Vector(float x, float y, float z);
        explicit Vector(const Point &point);
        explicit Vector(const Normal &normal);
        explicit Vector(const VectorProxy &proxy);

        float x() const;
        float y() const;
        float z() const;

        Vector normalize() const;
        float magnitude() const;
        float magnitude2() const;

        Vector operator+(const Vector &b) const;
        Vector operator-(const Vector &b) const;
        float operator*(const Vector &b) const;
        Vector operator*(float b) const;
        Vector operator/(float b) const;
        Vector operator-() const;
        Vector operator%(const Vector &b) const;

        void writeProxy(VectorProxy &proxy) const;

        static Vector fromPolar(float phi, float theta, float r);

    private:
        float mX;
        float mY;
        float mZ;
    };

    Vector operator*(const BaseTransformation &transformation, const Vector &vector);
    Vector operator*(const Matrix &matrix, const Vector &vector);
    Vector operator*(const Vector &vector, const Matrix &matrix);
}

#endif
