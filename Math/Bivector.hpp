#ifndef MATH_BIVECTOR_HPP
#define MATH_BIVECTOR_HPP

#include "Math/Vector.hpp"
#include "Math/Vector2D.hpp"

#include "Proxies.hpp"

namespace Math {
    class Bivector
    {
    public:
        Bivector() = default;
        Bivector(const Math::Vector &u, const Math::Vector &v);
        Bivector(const BivectorProxy &proxy);

        const Math::Vector &u() const;
        const Math::Vector &v() const;

        Bivector operator*(float b) const;
        Bivector operator+(const Bivector &b) const;
        Bivector operator-(const Bivector &b) const;
        Bivector operator/(float b) const;

        Math::Vector operator*(const Math::Vector2D &b) const;

        void writeProxy(BivectorProxy &proxy) const;

    private:
        Math::Vector mU;
        Math::Vector mV;
    };

    Bivector operator*(const BaseTransformation &transformation, const Bivector &bivector);
}
#endif