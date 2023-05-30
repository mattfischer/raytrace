#ifndef RENDER_FRAMEBUFFER_HPP
#define RENDER_FRAMEBUFFER_HPP

#include "Object/Color.hpp"

#include <vector>

namespace Render {
    class Framebuffer
    {
    public:
        Framebuffer(unsigned int width, unsigned int height);

        unsigned int width() const;
        unsigned int height() const;
        const unsigned char *bits() const;

        void setPixel(unsigned int x, unsigned int y, const Object::Color &color);

    private:
        unsigned int mWidth;
        unsigned int mHeight;
        std::vector<unsigned char> mBits;
    };
}

#endif
