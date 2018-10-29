#ifndef OBJECT_ALBEDO_TEXTURE_HPP
#define OBJECT_ALBEDO_TEXTURE_HPP

#include "Object/Albedo/Base.hpp"

#include <string>
#include <vector>

namespace Object {
	namespace Albedo {
		class Texture : public Base
		{
		public:
			Texture(const std::string &filename);

			virtual Object::Color color(float u, float v) const;

		protected:
			std::vector<unsigned char> mBits;
			int mWidth;
			int mHeight;
		};
	}
}

#endif