#include "Object/PointLight.hpp"

#include "Math/Vector.hpp"
#include "Math/Transformation.hpp"

namespace Object {
	PointLight::PointLight(const Math::Point &position, const Object::Radiance &radiance)
		: mPosition(position), mRadiance(radiance)
	{
	}

	const Math::Point &PointLight::position() const
	{
		return mPosition;
	}

	const Object::Radiance &PointLight::radiance() const
	{
		return mRadiance;
	}
}