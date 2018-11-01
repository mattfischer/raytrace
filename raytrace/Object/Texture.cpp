#include "Object/Texture.hpp"

#include <algorithm>

namespace Object {
	TextureBase::TextureBase(int width, int height, int numChannels, std::vector<float> &&values)
	{
		std::unique_ptr<MipLevel> level = std::make_unique<MipLevel>();
		level->width = width;
		level->height = height;
		level->values = std::move(values);
		mMipMaps.push_back(std::move(level));

		generateMipMaps(numChannels);
	}

	void TextureBase::doSample(const Math::Point2D &samplePoint, const Math::Bivector2D &sampleProjection, int numChannels, float values[]) const
	{
		int x = samplePoint.u() * mMipMaps[0]->width;
		int y = samplePoint.v() * mMipMaps[0]->height;

		for (int i = 0; i < numChannels; i++) {
			values[i] = mMipMaps[0]->values[(y * mMipMaps[0]->width + x) * numChannels + i];
		}
	}

	void TextureBase::doGradient(const Math::Point2D &samplePoint, int numChannels, Math::Vector2D gradient[]) const
	{
		int x = samplePoint.u() * mMipMaps[0]->width;
		int y = samplePoint.v() * mMipMaps[0]->height;

		for (int i = 0; i < numChannels; i++) {
			float du = (mMipMaps[0]->values[(y * mMipMaps[0]->width + x + 1) * numChannels + i] - mMipMaps[0]->values[(y * mMipMaps[0]->width + x) * numChannels + i]) * mMipMaps[0]->width;
			float dv = (mMipMaps[0]->values[((y + 1) * mMipMaps[0]->width + x) * numChannels + i] - mMipMaps[0]->values[(y * mMipMaps[0]->width + x) * numChannels + i]) * mMipMaps[0]->height;
			gradient[i] = Math::Vector2D(du, dv);
		}
	}

	void TextureBase::generateMipMaps(int numChannels)
	{
		while (mMipMaps[mMipMaps.size() - 1]->width > 1 || mMipMaps[mMipMaps.size() - 1]->height > 1) {
			MipLevel &lastLevel = *mMipMaps[mMipMaps.size() - 1];

			int width = std::max(1, (lastLevel.width + 1) / 2);
			int height = std::max(1, (lastLevel.height + 1) / 2);
			std::vector<float> values(width * height * numChannels);

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int x0 = x * 2;
					int x1 = std::min(x0 + 1, lastLevel.width);
					int y0 = y * 2;
					int y1 = std::min(y0 + 1, lastLevel.height);

					for (int i = 0; i < numChannels; i++) {
						float s =
							lastLevel.values[(y0 * lastLevel.width + x0) * numChannels + i] +
							lastLevel.values[(y0 * lastLevel.width + x1) * numChannels + i] +
							lastLevel.values[(y1 * lastLevel.width + x0) * numChannels + i] +
							lastLevel.values[(y1 * lastLevel.width + x1) * numChannels + i];
						values[(y * width + x) * numChannels + i] = s / 4.0f;
					}
				}
			}

			std::unique_ptr<MipLevel> level = std::make_unique<MipLevel>();
			level->width = width;
			level->height = height;
			level->values = std::move(values);

			mMipMaps.push_back(std::move(level));
		}
	}
}