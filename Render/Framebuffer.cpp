#include "Framebuffer.hpp"

#include <cstring>

namespace Render {
    Framebuffer::Framebuffer(unsigned int width, unsigned int height)
        : mWidth(width), mHeight(height)
    {
        mBits.resize(mWidth * mHeight * 3);
        if(mBits.size() > 0) {
            std::memset(&mBits[0], 0, mBits.size());
        }
    }

    unsigned int Framebuffer::width() const
    {
        return mWidth;
    }

    unsigned int Framebuffer::height() const
    {
        return mHeight;
    }

    const unsigned char *Framebuffer::bits() const
    {
        return &mBits[0];
    }

    void Framebuffer::setPixel(unsigned int x, unsigned int y, const Object::Color &color)
    {
        mBits[(mWidth * y + x) * 3 + 0] = static_cast<unsigned char>(color.red() * 0xff);
        mBits[(mWidth * y + x) * 3 + 1] = static_cast<unsigned char>(color.green() * 0xff);
        mBits[(mWidth * y + x) * 3 + 2] = static_cast<unsigned char>(color.blue() * 0xff);
    }

    Object::Color Framebuffer::toneMap(const Object::Radiance &rad)
    {
        float red = rad.red() / (rad.red() + 1);
        float green = rad.green() / (rad.green() + 1);
        float blue = rad.blue() / (rad.blue() + 1);

        return Object::Color(red, green, blue);
    }
}
