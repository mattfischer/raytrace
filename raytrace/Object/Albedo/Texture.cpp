#include "Object/Albedo/Texture.hpp"

#include <fstream>

#include <windows.h>

namespace Object {
	namespace Albedo {
		Texture::Texture(std::unique_ptr<Object::Texture> texture)
			: mTexture(std::move(texture))
		{
		}

		Object::Color Texture::color(float u, float v) const
		{
			return mTexture->sample(u, v);
		}
	}
}