#include "Object/NormalMap.hpp"

namespace Object {
	NormalMap::NormalMap(std::unique_ptr<Object::Texture<3>> texture, float magnitude)
		: mTexture(std::move(texture))
	{
		mMagnitude = magnitude;
	}

	Math::Normal NormalMap::perturbNormal(const Math::Point2D &surfacePoint, const Math::Bivector2D &surfaceProjection, const Math::Normal &normal, const Math::Bivector &tangent) const
	{
		Math::Vector2D gradient[3];
		mTexture->gradient(surfacePoint, surfaceProjection, gradient);
		Math::Vector offset = tangent * gradient[0];
		return (normal - Math::Normal(offset) * mMagnitude).normalize();
	}
}