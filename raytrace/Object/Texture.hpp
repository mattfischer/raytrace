#ifndef OBJECT_TEXTURE_HPP
#define OBJECT_TEXTURE_HPP

#include "Object/Color.hpp"
#include "Math/Point2D.hpp"

#include <vector>

namespace Object {
	class Texture
	{
	public:
		Texture(int width, int height, std::vector<unsigned char> &&bits);

		Object::Color sample(const Math::Point2D &samplePoint) const;

	protected:
		std::vector<unsigned char> mBits;
		int mWidth;
		int mHeight;
	};
}
#endif