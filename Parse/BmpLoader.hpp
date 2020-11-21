#ifndef PARSE_BMP_LOADER_HPP
#define PARSE_BMP_LOADER_HPP

#include "Object/Texture.hpp"

#include <memory>
#include <string>

namespace Parse {
    class BmpLoader
    {
    public:
        static std::unique_ptr<Object::Texture<3>> load(const std::string &filename);
    };
}
#endif