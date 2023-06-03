#ifndef MATH_ORTHONORMAL_BASIS_HPP
#define MATH_ORTHONORMAL_BASIS_HPP

#include "Math/Vector.hpp"
#include "Math/Normal.hpp"
#include "Math/Matrix.hpp"

namespace Math {
    class OrthonormalBasis
    {
    public:
        OrthonormalBasis() = default;
        OrthonormalBasis(const Math::Vector &z);
        OrthonormalBasis(const Math::Normal &z);
        OrthonormalBasis(const Math::Vector &x, const Math::Vector &y, const Math::Vector &z);

        Math::Vector worldToLocal(const Math::Vector &world) const;
        Math::Vector localToWorld(const Math::Vector &local) const;

    private:
        Math::Matrix mMatrix;
    };
}
#endif