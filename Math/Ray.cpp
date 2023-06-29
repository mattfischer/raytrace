#include "Math/Ray.hpp"

namespace Math {
    Ray::Ray(const Point &origin, const Vector &direction)
    : mOrigin(origin), mDirection(direction)
    {
    }

    Ray::Ray(const RayProxy &proxy)
    {
        mOrigin = Point(proxy.origin);
        mDirection = Vector(proxy.direction);
    }

    Ray operator*(const BaseTransformation &transformation, const Ray &ray)
    {
        return Ray(transformation * ray.origin(), transformation * ray.direction());
    }

    const Point &Ray::origin() const
    {
        return mOrigin;
    }

    const Vector &Ray::direction() const
    {
        return mDirection;
    }

    void Ray::writeProxy(RayProxy &proxy) const
    {
        mOrigin.writeProxy(proxy.origin);
        mDirection.writeProxy(proxy.direction);
    }
}