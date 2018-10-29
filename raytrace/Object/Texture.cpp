#include "Object/Texture.hpp"

namespace Object {
	Texture::Texture(int width, int height, std::vector<unsigned char> &&bits)
		: mWidth(width), mHeight(height), mBits(std::move(bits))
	{
	}

	Object::Color Texture::sample(float u, float v) const
	{
		int x = u * mWidth;
		int y = v * mHeight;

		unsigned char r = mBits[(y * mWidth + x) * 3 + 2];
		unsigned char g = mBits[(y * mWidth + x) * 3 + 1];
		unsigned char b = mBits[(y * mWidth + x) * 3 + 0];

		return Object::Color((float)r / 0xff, (float)g / 0xff, (float)b / 0xff);
	}
}