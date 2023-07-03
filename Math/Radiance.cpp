#include "Math/Radiance.hpp"
#include "Math/Color.hpp"

#include <cmath>
#include <algorithm>

namespace Math {
    Radiance::Radiance()
    {
        mRed = 0;
        mGreen = 0;
        mBlue = 0;
    }

    Radiance::Radiance(float red, float green, float blue)
    {
        mRed = red;
        mGreen = green;
        mBlue = blue;
    }

    Radiance::Radiance(RadianceProxy &proxy)
    {
        mRed = proxy.coords[0];
        mGreen = proxy.coords[1];
        mBlue = proxy.coords[2];
    }

    float Radiance::red() const
    {
        return mRed;
    }

    float Radiance::green() const
    {
        return mGreen;
    }

    float Radiance::blue() const
    {
        return mBlue;
    }

    float Radiance::magnitude() const
    {
        return std::sqrt(red() * red() + green() * green() + blue() * blue());
    }

    Radiance Radiance::operator+(const Radiance &b) const
    {
        return Radiance(mRed + b.mRed, mGreen + b.mGreen, mBlue + b.mBlue);
    }

    Radiance &Radiance::operator+=(const Radiance &b)
    {
        mRed += b.mRed;
        mGreen += b.mGreen;
        mBlue += b.mBlue;

        return *this;
    }

    Radiance Radiance::operator/(float b) const
    {
        return Radiance(mRed / b, mGreen / b, mBlue / b);
    }

    Radiance Radiance::operator*(const Radiance &b) const
    {
        return Radiance(mRed * b.mRed, mGreen * b.mGreen, mBlue * b.mBlue);
    }

    Radiance Radiance::operator*(float b) const
    {
        return Radiance(mRed * b, mGreen * b, mBlue * b);
    }

    Radiance Radiance::operator*(const Color &b) const
    {
        return Radiance(mRed * b.red(), mGreen * b.green(), mBlue * b.blue());
    }

    Radiance Radiance::operator-(const Radiance &b) const
    {
        return Radiance(mRed - b.mRed, mGreen - b.mGreen, mBlue - b.mBlue);
    }

    Radiance Radiance::clamp() const
    {
        return Radiance(std::max(mRed, 0.0f), std::max(mGreen, 0.0f), std::max(mBlue, 0.0f));
    }

    void Radiance::writeProxy(RadianceProxy &proxy) const
    {
        proxy.coords[0] = mRed;
        proxy.coords[1] = mGreen;
        proxy.coords[2] = mBlue;
    }
}