#ifndef OBJECT_LIGHT_HPP
#define OBJECT_LIGHT_HPP

#include "Math/Point.hpp"
#include "Object/Radiance.hpp"

#include <memory>

namespace Object {
	class Light
	{
	public:
		Light(const Math::Point &position, const Object::Radiance &radiance);

		const Math::Point &position() const;
		const Object::Radiance &radiance() const;

	private:
		Math::Point mPosition;
		Object::Radiance mRadiance;
	};
}

#endif