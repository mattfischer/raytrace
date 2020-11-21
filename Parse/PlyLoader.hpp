#ifndef PARSE_PLY_LOADER_HPP
#define PARSE_PLY_LOADER_HPP

#include "Object/Shape/Base.hpp"

#include <memory>

namespace Parse {
    class PlyLoader
    {
    public:
        static std::unique_ptr<Object::Shape::Base> load(const std::string &filename);
    };
}
#endif