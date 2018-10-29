#ifndef OBJECT_ALBEDO_TEXTURE_HPP
#define OBJECT_ALBEDO_TEXTURE_HPP

#include "Object/Albedo/Base.hpp"

#include "Object/Texture.hpp"

#include <memory>

namespace Object {
	namespace Albedo {
		class Texture : public Base
		{
		public:
			Texture(std::unique_ptr<Object::Texture> texture);

			virtual Object::Color color(float u, float v) const;

		protected:
			std::unique_ptr<Object::Texture> mTexture;
		};
	}
}

#endif