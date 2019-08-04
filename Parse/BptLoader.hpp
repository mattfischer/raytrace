#ifndef PARSE_BPT_LOADER_HPP
#define PARSE_BPT_LOADER_HPP

#include "Object/Shape/Base.hpp"

#include <memory>

namespace Parse {
	class BptLoader
	{
	public:
		static std::unique_ptr<Object::Shape::Base> load(const std::string &filename);
	};
}
#endif