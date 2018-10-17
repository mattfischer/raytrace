#include "Object/Light.hpp"

#include "Math/Vector.hpp"
#include "Math/Transformation.hpp"

namespace Object {
	Light::Light(const Math::Point &position, const Object::Radiance &radiance)
		: mPosition(position), mRadiance(radiance)
	{
	}

	const Math::Point &Light::position() const
	{
		return mPosition;
	}

	const Object::Radiance &Light::radiance() const
	{
		return mRadiance;
	}
}