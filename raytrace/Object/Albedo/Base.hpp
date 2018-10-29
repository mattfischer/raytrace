#ifndef OBJECT_ALBEDO_BASE_HPP
#define OBJECT_ALBEDO_BASE_HPP

#include "Object/Color.hpp"

#include "Math/Point.hpp"

namespace Object {
	namespace Albedo {
		class Base
		{
		public:
			virtual Object::Color color(float u, float v) const = 0;
		};
	}
}

#endif