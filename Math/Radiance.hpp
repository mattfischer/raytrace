#ifndef MATH_RADIANCE_HPP
#define MATH_RADIANCE_HPP

#include "Math/Color.hpp"

#include "Proxies.hpp"

namespace Math {
    class Radiance
    {
    public:
        Radiance();
        Radiance(float red, float green, float blue);
        Radiance(RadianceProxy &proxy);

        float red() const;
        float green() const;
        float blue() const;

        float magnitude() const;

        Radiance operator+(const Radiance &b) const;
        Radiance &operator+=(const Radiance &b);
        Radiance operator/(float b) const;
        Radiance operator*(const Radiance &b) const;
        Radiance operator*(float b) const;
        Radiance operator*(const Color &b) const;
        Radiance operator-(const Radiance &b) const;

        Radiance clamp() const;

        void writeProxy(RadianceProxy &proxy) const;

    protected:
        float mRed;
        float mGreen;
        float mBlue;
    };
}

#endif
