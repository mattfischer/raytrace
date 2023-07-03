#ifndef RENDER_FRAMEBUFFER_HPP
#define RENDER_FRAMEBUFFER_HPP

#include "Math/Color.hpp"
#include "Math/Radiance.hpp"

#include <vector>

namespace Render {
    class Framebuffer
    {
    public:
        Framebuffer(unsigned int width, unsigned int height);

        unsigned int width() const;
        unsigned int height() const;
        const unsigned char *bits() const;

        void setPixel(unsigned int x, unsigned int y, const Math::Color &color);

        static Math::Color toneMap(const Math::Radiance &rad);

    private:
        unsigned int mWidth;
        unsigned int mHeight;
        std::vector<unsigned char> mBits;
    };
}

#endif
