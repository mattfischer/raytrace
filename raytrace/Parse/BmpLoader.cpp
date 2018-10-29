#include "Parse/BmpLoader.hpp"

#include <fstream>
#include <windows.h>

namespace Parse {
	std::unique_ptr<Object::Texture> BmpLoader::load(const std::string &filename)
	{
		std::ifstream file(filename.c_str(), std::ios_base::binary);

		BITMAPFILEHEADER bfh;
		BITMAPINFOHEADER bih;

		file.read((char*)&bfh, sizeof(bfh));
		file.read((char*)&bih, sizeof(bih));
		file.seekg(bfh.bfOffBits);

		int width = bih.biWidth;
		int height = bih.biHeight;
		unsigned int size = width * height * 3;
		std::vector<unsigned char> bits(size);

		file.read((char*)&bits[0], size);

		return std::make_unique<Object::Texture>(width, height, std::move(bits));
	}
}