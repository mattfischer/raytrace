#define _USE_MATH_DEFINES
#include "Object/Texture.hpp"

#include <algorithm>

namespace Object {
	TextureBase::TextureBase(int width, int height, int numChannels, std::vector<float> &&values)
	{
		std::unique_ptr<MipLevel> level = createBaseLevel(width, height, numChannels, std::move(values));
		mMipMaps.push_back(std::move(level));

		generateMipMaps(numChannels);
	}

	int TextureBase::selectMipLevel(const Math::Bivector2D &sampleProjection) const
	{
		int level = 0;
		float projectionSize = std::sqrt(std::abs(sampleProjection.u() % sampleProjection.v()));

		for (int i = 0; i < mMipMaps.size(); i++) {
			float texelSize = std::max(1.0f / mMipMaps[i]->width, 1.0f / mMipMaps[i]->height);
			if (texelSize >= projectionSize || i == mMipMaps.size() - 1) {
				level = i;
				break;
			}
		}

		return level;
	}

	void TextureBase::doSample(const Math::Point2D &samplePoint, const Math::Bivector2D &sampleProjection, int numChannels, float values[]) const
	{
		int l = selectMipLevel(sampleProjection);
		const std::unique_ptr<MipLevel> &level = mMipMaps[l];

		float fx = samplePoint.u() * (level->width - 1);
		float fy = samplePoint.v() * (level->height - 1);

		int x = std::floor(fx);
		int y = std::floor(fy);
		float dx = fx - x;
		float dy = fy - y;

		for (int i = 0; i < numChannels; i++) {
			values[i] = (1 - dx) * (1 - dy) * level->values[(y * level->width + x) * numChannels + i] +
				dx * (1 - dy) * level->values[(y * level->width + x + 1) * numChannels + i] +
				(1 - dx) * dy * level->values[((y + 1) * level->width + x) * numChannels + i] +
				dx * dy * level->values[((y + 1) * level->width + x + 1) * numChannels + i];
		}
	}

	void TextureBase::doGradient(const Math::Point2D &samplePoint, const Math::Bivector2D &sampleProjection, int numChannels, Math::Vector2D gradient[]) const
	{
		int l = selectMipLevel(sampleProjection);
		const std::unique_ptr<MipLevel> &level = mMipMaps[l];

		int x = samplePoint.u() * level->width;
		int y = samplePoint.v() * level->height;

		for (int i = 0; i < numChannels; i++) {
			float du = (level->values[(y * level->width + x + 1) * numChannels + i] - level->values[(y * level->width + x) * numChannels + i]) * level->width;
			float dv = (level->values[((y + 1) * level->width + x) * numChannels + i] - level->values[(y * level->width + x) * numChannels + i]) * level->height;
			gradient[i] = Math::Vector2D(du, dv);
		}
	}

	int nextPowerOfTwo(int x)
	{
		int pot = 0;
		int y = x;
		for (int i = 0; ; i++) {
			if (y == 1) {
				pot = 1 << i;
				break;
			}
			else {
				y = y >> 1;
			}
		}

		if(pot != x) {
			pot *= 2;
		}

		return pot;
	}
	std::unique_ptr<TextureBase::MipLevel> TextureBase::createBaseLevel(int width, int height, int numChannels, std::vector<float> &&values)
	{
		int widthPot = nextPowerOfTwo(width);
		if (widthPot != width) {
			std::vector<float> newValues(widthPot * height * numChannels);
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < widthPot; x++) {
					float sx = (float)(x * width) / widthPot;
					const int A = 3;
					for (int i = 0; i < numChannels; i++) {
						newValues[(y * widthPot + x) * numChannels + i] = 0;
					}

					for (int xx = std::ceil(sx) - A; xx <= std::floor(sx) + A; xx++) {
						float u = sx - xx;
						float f = 0;
						if (u == 0) {
							f = 1;
						} else if(std::abs(u) <= A) {
							f = A * std::sin(M_PI * u) * std::sin(M_PI * u / A) / (M_PI * M_PI * u * u);
						}
						for (int i = 0; i < numChannels; i++) {
							float c = 0;
							if (xx >= 0 && xx < width) {
								c = values[(y * width + xx) * numChannels + i];
							}
							newValues[(y * widthPot + x) * numChannels + i] += c * f;
						}
					}
				}
			}
			values = std::move(newValues);
			width = widthPot;
		}

		int heightPot = nextPowerOfTwo(height);
		if (heightPot != height) {
			std::vector<float> newValues(width * heightPot * numChannels);
			for (int y = 0; y < heightPot; y++) {
				for (int x = 0; x < width; x++) {
					float sy = (float)(y * height) / heightPot;
					const int A = 3;
					for (int i = 0; i < numChannels; i++) {
						newValues[(y * width + x) * numChannels + i] = 0;
					}

					for (int yy = std::ceil(sy) - A; yy <= std::floor(sy) + A; yy++) {
						float u = sy - yy;
						float f = 0;
						if (u == 0) {
							f = 1;
						}
						else if (std::abs(u) <= A) {
							f = A * std::sin(M_PI * u) * std::sin(M_PI * u / A) / (M_PI * M_PI * u * u);
						}
						for (int i = 0; i < numChannels; i++) {
							float c = 0;
							if (yy >= 0 && yy < height) {
								c = values[(yy * width + x) * numChannels + i];
							}
							newValues[(y * width + x) * numChannels + i] += c * f;
						}
					}
				}
			}
			values = std::move(newValues);
			height = heightPot;
		}

		std::unique_ptr<MipLevel> level = std::make_unique<MipLevel>();
		level->width = width;
		level->height = height;
		level->values = std::move(values);

		return level;
	}

	void TextureBase::generateMipMaps(int numChannels)
	{
		while (mMipMaps[mMipMaps.size() - 1]->width > 1 || mMipMaps[mMipMaps.size() - 1]->height > 1) {
			MipLevel &lastLevel = *mMipMaps[mMipMaps.size() - 1];

			int width = std::max(1, lastLevel.width / 2);
			int height = std::max(1, lastLevel.height / 2);
			std::vector<float> values(width * height * numChannels);

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int x0 = x * 2;
					int x1 = x0 + 1;
					int y0 = y * 2;
					int y1 = y0 + 1;

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