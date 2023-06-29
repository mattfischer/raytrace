#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Math/Transformation.hpp"

namespace Math {
    Point::Point()
    {
        mX = mY = mZ = 0;
    }

    Point::Point(float x, float y, float z)
    {
        mX = x;
        mY = y;
        mZ = z;
    }

    Point::Point(const Vector &c)
    {
        mX = c.x();
        mY = c.y();
        mZ = c.z();
    }

    Point::Point(const PointProxy &proxy)
    {
        mX = proxy.coords[0];
        mY = proxy.coords[1];
        mZ = proxy.coords[2];
    }

    float Point::x() const
    {
        return mX;
    }

    float Point::y() const
    {
        return mY;
    }

    float Point::z() const
    {
        return mZ;
    }

    Point Point::operator+(const Vector &b) const
    {
        return Point(x() + b.x(), y() + b.y(), z() + b.z());
    }

    Point Point::operator-(const Vector &b) const
    {
        return Point(x() - b.x(), y() - b.y(), z() - b.z());
    }

    Vector Point::operator-(const Point &b) const
    {
        return Vector(x() - b.x(), y() - b.y(), z() - b.z());
    }

    Point operator*(const BaseTransformation &transformation, const Point &point)
    {
        return Point(transformation.matrix() * point);
    }

    Point operator*(const Matrix &matrix, const Point &point)
    {
        if (matrix.identity()) return point;

        float x = matrix(0, 0) * point.x() + matrix(1, 0) * point.y() + matrix(2, 0) * point.z() + matrix(3, 0);
        float y = matrix(0, 1) * point.x() + matrix(1, 1) * point.y() + matrix(2, 1) * point.z() + matrix(3, 1);
        float z = matrix(0, 2) * point.x() + matrix(1, 2) * point.y() + matrix(2, 2) * point.z() + matrix(3, 2);

        return Point(x, y, z);
    }

    void Point::writeProxy(PointProxy &proxy) const
    {
        proxy.coords[0] = x();
        proxy.coords[1] = y();
        proxy.coords[2] = z();
    }
}