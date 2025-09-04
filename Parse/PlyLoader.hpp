#ifndef PARSE_PLY_LOADER_HPP
#define PARSE_PLY_LOADER_HPP

#include "Object/Shape.hpp"

#include <memory>
#include <string>

namespace Parse {
    class PlyLoader
    {
    public:
        static std::unique_ptr<Object::Shape> load(const std::string &filename);
    };
}
#endif
