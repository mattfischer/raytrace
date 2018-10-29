#include "Object/Albedo/Texture.hpp"

#include <fstream>

#include <windows.h>

namespace Object {
	namespace Albedo {
		Texture::Texture(const std::string &filename)
		{
			std::ifstream file(filename.c_str(), std::ios_base::binary);

			BITMAPFILEHEADER bfh;
			BITMAPINFOHEADER bih;

			file.read((char*)&bfh, sizeof(bfh));
			file.read((char*)&bih, sizeof(bih));

			mWidth = bih.biWidth;
			mHeight = bih.biHeight;

			file.seekg(bfh.bfOffBits);

			unsigned int size = bih.biWidth * bih.biHeight * 3;
			mBits.resize(size);

			file.read((char*)&mBits[0], size);
		}

		Object::Color Texture::color(float u, float v) const
		{
			int x = u * mWidth;
			int y = v * mHeight;
			
			unsigned char r = mBits[(y * mWidth + x) * 3 + 2];
			unsigned char g = mBits[(y * mWidth + x) * 3 + 1];
			unsigned char b = mBits[(y * mWidth + x) * 3 + 0];

			return Object::Color((float)r / 0xff, (float)g / 0xff, (float)b / 0xff);
		}
	}
}