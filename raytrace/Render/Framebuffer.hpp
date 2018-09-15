#ifndef RENDER_FRAMEBUFFER_HPP
#define RENDER_FRAMEBUFFER_HPP

#include "Object/Color.hpp"

#include <vector>

namespace Render {

class Framebuffer
{
public:
	Framebuffer(int width, int height);

	int width() const;
	int height() const;
	const unsigned char *bits() const;

	void setPixel(int x, int y, const Object::Color &color);

private:
	int mWidth;
	int mHeight;
	std::vector<unsigned char> mBits;
};

}

#endif