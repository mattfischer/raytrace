#include "Object/Albedo/Texture.hpp"

#include <fstream>

#include <windows.h>

namespace Object {
	namespace Albedo {
		Texture::Texture(std::unique_ptr<Object::Texture> texture)
			: mTexture(std::move(texture))
		{
		}

		Object::Color Texture::color(const Math::Point2D &surfacePoint) const
		{
			return mTexture->sample(surfacePoint);
		}
	}
}