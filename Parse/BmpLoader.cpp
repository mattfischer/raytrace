#include "Parse/BmpLoader.hpp"

#include <fstream>
#include <windows.h>

namespace Parse {
    std::unique_ptr<Object::Texture<3>> BmpLoader::load(const std::string &filename)
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

        std::vector<float> values(size);
        for (int i = 0; i < width * height; i++) {
            values[i * 3 + 0] = (float)bits[i * 3 + 2] / 0xff;
            values[i * 3 + 1] = (float)bits[i * 3 + 1] / 0xff;
            values[i * 3 + 2] = (float)bits[i * 3 + 0] / 0xff;
        }

        return std::make_unique<Object::Texture<3>>(width, height, std::move(values));
    }
}