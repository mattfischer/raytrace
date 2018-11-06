#include "Object/Albedo/Texture.hpp"

#include <fstream>

#include <windows.h>

namespace Object {
	namespace Albedo {
		Texture::Texture(std::unique_ptr<Object::Texture<3>> texture)
			: mTexture(std::move(texture))
		{
			mTexture->generateMipMaps();
		}

		Object::Color Texture::color(const Math::Point2D &surfacePoint, const Math::Bivector2D &surfaceProjection) const
		{
			Object::Texture<3>::Value value = mTexture->sample(surfacePoint, surfaceProjection);

			return Object::Color(value.channels[0], value.channels[1], value.channels[2]);
		}
	}
}