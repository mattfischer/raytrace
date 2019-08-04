#include "Framebuffer.hpp"

namespace Render {
	Framebuffer::Framebuffer(int width, int height)
		: mWidth(width), mHeight(height)
	{
		mBits.resize(mWidth * mHeight * 3);
		if(mBits.size() > 0) {
			std::memset(&mBits[0], 0, mBits.size());
		}
	}

	int Framebuffer::width() const
	{
		return mWidth;
	}

	int Framebuffer::height() const
	{
		return mHeight;
	}

	const unsigned char *Framebuffer::bits() const
	{
		return &mBits[0];
	}

	void Framebuffer::setPixel(int x, int y, const Object::Color &color)
	{
		mBits[(mWidth * y + x) * 3 + 0] = color.red() * 0xff;
		mBits[(mWidth * y + x) * 3 + 1] = color.green() * 0xff;
		mBits[(mWidth * y + x) * 3 + 2] = color.blue() * 0xff;
	}
}
