#include "Object/Color.hpp"

#include <algorithm>

namespace Object {
    Color::Color()
    {
        mRed = 0;
        mGreen = 0;
        mBlue = 0;
    }

    Color::Color(float red, float green, float blue)
    {
        mRed = red;
        mGreen = green;
        mBlue = blue;
    }

    float Color::red() const
    {
        return mRed;
    }

    float Color::green() const
    {
        return mGreen;
    }

    float Color::blue() const
    {
        return mBlue;
    }

    Color Color::operator+(const Color &b) const
    {
        return Color(mRed + b.mRed, mGreen + b.mGreen, mBlue + b.mBlue);
    }

    Color Color::operator-(const Color &b) const
    {
        return Color(mRed - b.mRed, mGreen - b.mGreen, mBlue - b.mBlue);
    }

    Color &Color::operator+=(const Color &b)
    {
        mRed += b.mRed;
        mGreen += b.mGreen;
        mBlue += b.mBlue;

        return *this;
    }

    Color Color::operator/(float b) const
    {
        return Color(mRed / b, mGreen / b, mBlue / b);
    }

    Color Color::operator*(const Color &b) const
    {
        return Color(mRed * b.mRed, mGreen * b.mGreen, mBlue * b.mBlue);
    }

    Color Color::operator*(float b) const
    {
        return Color(mRed * b, mGreen * b, mBlue * b);
    }

    float Color::magnitude2() const
    {
        return mRed * mRed + mGreen * mGreen + mBlue * mBlue;
    }

    float Color::maximum() const
    {
        return std::max(std::max(mRed, mGreen), mBlue);
    }

    void Color::writeProxy(ColorProxy &proxy) const
    {
        proxy.coords[0] = mRed;
        proxy.coords[1] = mGreen;
        proxy.coords[2] = mBlue;
    }
}