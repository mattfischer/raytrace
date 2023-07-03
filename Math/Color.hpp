#ifndef MATH_COLOR_HPP
#define MATH_COLOR_HPP

#include "Math/CLProxies.hpp"

namespace Math {
    class Color
    {
    public:
        Color();
        Color(float red, float green, float blue);

        float red() const;
        float green() const;
        float blue() const;

        Color operator+(const Color &b) const;
        Color operator-(const Color &b) const;
        Color &operator+=(const Color &b);
        Color operator/(float b) const;
        Color operator*(const Color &b) const;
        Color operator*(float b) const;

        float magnitude2() const;
        float maximum() const;

        void writeProxy(ColorProxy &proxy) const;

    protected:
        float mRed;
        float mGreen;
        float mBlue;
    };
}

#endif
