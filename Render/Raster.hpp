#ifndef RENDER_RASTER_HPP
#define RENDER_RASTER_HPP

#include <vector>

namespace Render {
	template <typename T>
	class Raster {
	public:
        Raster(unsigned int width, unsigned int height)
		{
			mWidth = width;
			mHeight = height;
			mElements.resize(mWidth * mHeight);
		}

        unsigned int width() const { return mWidth; }
        unsigned int height() const { return mHeight; }

        void set(unsigned int x, unsigned int y, const T &value) { mElements[mWidth * y + x] = value; }
        const T &get(unsigned int x,unsigned  int y) const { return mElements.at(mWidth * y + x); }

	private:
        unsigned int mWidth;
        unsigned int mHeight;
		std::vector<T> mElements;
	};
}
#endif
