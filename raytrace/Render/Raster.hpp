#ifndef RENDER_RASTER_HPP
#define RENDER_RASTER_HPP

#include <vector>

namespace Render {
	template <typename T>
	class Raster {
	public:
		Raster(int width, int height)
		{
			mWidth = width;
			mHeight = height;
			mElements.resize(mWidth * mHeight);
		}

		int width() const { return mWidth; }
		int height() const { return mHeight; }

		void set(int x, int y, const T &value) { mElements[mWidth * y + x] = value; }
		const T &get(int x, int y) const { return mElements.at(mWidth * y + x); }

	private:
		int mWidth;
		int mHeight;
		std::vector<T> mElements;
	};
}
#endif