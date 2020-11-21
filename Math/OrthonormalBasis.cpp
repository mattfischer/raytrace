#include "Math/OrthonormalBasis.hpp"

#include <cmath>

namespace Math {
    OrthonormalBasis::OrthonormalBasis(const Math::Vector &z)
    {
        float sign = std::copysignf(1.0f, z.z());
        const float a = -1.0f / (sign + z.z());
        const float b = z.x() *    z.y() *    a;
        Math::Vector x(1.0f + sign * z.x() * z.x() * a, sign * b, -sign * z.x());
        Math::Vector y(b, sign + z.y() * z.y() * a, -z.y());

        mMatrix = Math::Matrix(x.x(), y.x(), z.x(), 0,
                               x.y(), y.y(), z.y(), 0,
                               x.z(), y.z(), z.z(), 0,
                               0,     0,     0,     1);
    }

    OrthonormalBasis::OrthonormalBasis(const Math::Normal &normal)
        : OrthonormalBasis(Math::Vector(normal))
    {
    }

    Math::Vector OrthonormalBasis::worldToLocal(const Math::Vector &world) const
    {
        return Math::Vector(world * mMatrix);
    }

    Math::Vector OrthonormalBasis::localToWorld(const Math::Vector &local) const
    {
        return Math::Vector(mMatrix * local);
    }
}