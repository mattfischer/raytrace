#ifndef MATH_RAY_HPP
#define MATH_RAY_HPP

#include "Math/Point.hpp"
#include "Math/Vector.hpp"

#include "Proxies.hpp"

namespace Math {
    class Ray
    {
    public:
        Ray() = default;
        Ray(const Point &origin, const Vector &direction);

        const Point &origin() const;
        const Vector &direction() const;

        void writeProxy(RayProxy &proxy) const;

    protected:
        Point mOrigin;
        Vector mDirection;
    };

    Ray operator*(const BaseTransformation &transformation, const Ray &ray);
}

#endif
