#ifndef PARSE_BPT_LOADER_HPP
#define PARSE_BPT_LOADER_HPP

#include "Object/Shape.hpp"

#include <memory>
#include <string>

namespace Parse {
    class BptLoader
    {
    public:
        static std::unique_ptr<Object::Shape> load(const std::string &filename);
    };
}
#endif
