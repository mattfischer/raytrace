#ifndef PARSE_PARSER_HPP
#define PARSE_PARSER_HPP

#include "Object/Scene.hpp"

#include <memory>
#include <string>

namespace Parse {
    class Parser
    {
    public:
        static std::unique_ptr<Object::Scene> parse(const std::string &filename);
    };
}

#endif