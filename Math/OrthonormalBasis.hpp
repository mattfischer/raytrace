#ifndef MATH_ORTHONORMAL_BASIS_HPP
#define MATH_ORTHONORMAL_BASIS_HPP

#include "Math/Vector.hpp"
#include "Math/Normal.hpp"
#include "Math/Matrix.hpp"

namespace Math {
    class OrthonormalBasis
    {
    public:
        OrthonormalBasis(const Math::Vector &z);
        OrthonormalBasis(const Math::Normal &z);

        Math::Vector worldToLocal(const Math::Vector &world) const;
        Math::Vector localToWorld(const Math::Vector &local) const;

    private:
        Math::Matrix mMatrix;
    };
}
#endif