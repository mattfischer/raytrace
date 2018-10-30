#include "Object/NormalMap.hpp"

namespace Object {
	NormalMap::NormalMap(std::unique_ptr<Object::Texture<3>> texture, float magnitude)
		: mTexture(std::move(texture))
	{
		mMagnitude = magnitude;
	}

	Math::Normal NormalMap::perturbNormal(const Math::Point2D &surfacePoint, const Math::Normal &normal, const Math::Vector &du, const Math::Vector &dv) const
	{
		Object::Texture<3>::Value textureDu, textureDv;
		mTexture->gradient(surfacePoint, textureDu, textureDv);
		Math::Vector offset = du * textureDu.channels[0] + dv * textureDv.channels[0];
		return (normal - Math::Normal(offset) * mMagnitude).normalize();
	}
}