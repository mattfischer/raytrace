#ifndef OBJECT_ALBEDO_BASE_HPP
#define OBJECT_ALBEDO_BASE_HPP

#include "Object/Color.hpp"

#include "Math/Point.hpp"

#include "Parse/AST.h"

#include <memory>

namespace Object {
	namespace Albedo {
		class Base
		{
		public:
			static std::unique_ptr<Base> fromAst(AST *ast);

			virtual Object::Color color(const Math::Point &point) const = 0;
		};
	}
}

#endif