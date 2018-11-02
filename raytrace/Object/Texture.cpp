#define _USE_MATH_DEFINES
#include "Object/Texture.hpp"

#include <algorithm>

namespace Object {
	TextureBase::TextureBase(int width, int height, int numChannels, std::vector<float> &&values)
	{
		std::unique_ptr<MipLevel> level = createBaseLevel(width, height, numChannels, std::move(values));
		mMipMaps.push_back(std::move(level));

		generateMipMaps();
	}

	float TextureBase::selectMipLevel(const Math::Bivector2D &sampleProjection) const
	{
		float projectionSize = std::sqrt(std::abs(sampleProjection.u() % sampleProjection.v()));

		float level = mMipMaps.size() - 1 + std::log2f(projectionSize);
		return std::max(0.0f, std::min(level, (float)mMipMaps.size() - 1));
	}

	void TextureBase::doSample(const Math::Point2D &samplePoint, const Math::Bivector2D &sampleProjection, float values[]) const
	{
		float lf = selectMipLevel(sampleProjection);
		int l = std::floor(lf);
		const std::unique_ptr<MipLevel> &level0 = mMipMaps[l];
		const std::unique_ptr<MipLevel> &level1 = mMipMaps[std::min(l + 1, (int)mMipMaps.size() - 1)];
		float dl = lf - l;

		float fx0 = samplePoint.u() * (level0->width() - 1);
		float fy0 = samplePoint.v() * (level0->height() - 1);
		float fx1 = samplePoint.u() * (level1->width() - 1);
		float fy1 = samplePoint.v() * (level1->height() - 1);

		int x0 = std::floor(fx0);
		int y0 = std::floor(fy0);
		int x1 = std::floor(fx1);
		int y1 = std::floor(fy1);

		float dx0 = fx0 - x0;
		float dy0 = fy0 - y0;
		float dx1 = fx1 - x1;
		float dy1 = fy1 - y1;

		for (int i = 0; i < level0->numChannels(); i++) {
			float v0 = (1 - dx0) * (1 - dy0) * level0->at(x0, y0, i) +
				dx0 * (1 - dy0) * level0->at(x0 + 1, y0, i) +
				(1 - dx0) * dy0 * level0->at(x0, y0 + 1, i) +
				dx0 * dy0 * level0->at(x0 + 1, y0 + 1, i);

			float v1 = (1 - dx1) * (1 - dy1) * level1->at(x1, y1, i) +
				dx1 * (1 - dy1) * level1->at(x1 + 1, y1, i) +
				(1 - dx1) * dy1 * level1->at(x1, y1 + 1, i) +
				dx1 * dy1 * level1->at(x1 + 1, y1 + 1, i);

			values[i] = v0 * (1 - dl) + v1 * dl;
		}
	}

	void TextureBase::doGradient(const Math::Point2D &samplePoint, const Math::Bivector2D &sampleProjection, Math::Vector2D gradient[]) const
	{
		float lf = selectMipLevel(sampleProjection);
		int l = std::floor(lf);
		const std::unique_ptr<MipLevel> &level0 = mMipMaps[l];
		const std::unique_ptr<MipLevel> &level1 = mMipMaps[std::min(l + 1, (int)mMipMaps.size() - 1)];
		float dl = lf - l;

		int x0 = samplePoint.u() * level0->width();
		int y0 = samplePoint.v() * level0->height();
		int x1 = samplePoint.u() * level1->width();
		int y1 = samplePoint.v() * level1->height();

		for (int i = 0; i < level0->numChannels(); i++) {
			float du0 = (level0->at(x0 + 1, y0, i) - level0->at(x0, y0, i)) * level0->width();
			float dv0 = (level0->at(x0, y0 + 1, i) - level0->at(x0, y0, i)) * level0->height();
			float du1 = (level1->at(x1 + 1, y1, i) - level1->at(x1, y1, i)) * level1->width();
			float dv1 = (level1->at(x1, y1 + 1, i) - level1->at(x1, y1, i)) * level1->height();

			gradient[i] = Math::Vector2D(du0, dv0) * (1 - dl) + Math::Vector2D(du1, dv1) * dl;
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
		std::unique_ptr<MipLevel> level = std::make_unique<MipLevel>(width, height, numChannels, std::move(values));

		int widthPot = nextPowerOfTwo(width);
		if (widthPot != width) {
			std::unique_ptr<MipLevel> newLevel = std::make_unique<MipLevel>(widthPot, height, numChannels);

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < widthPot; x++) {
					float sx = (float)(x * width) / widthPot;
					const int A = 3;
					for (int i = 0; i < numChannels; i++) {
						newLevel->at(x, y, i) = 0;
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
							float c = (xx >= 0 && xx < width) ? level->at(xx, y, i) : 0;
							newLevel->at(x, y, i) += c * f;
						}
					}
				}
			}
			level = std::move(newLevel);
		}

		int heightPot = nextPowerOfTwo(height);
		if (heightPot != height) {
			std::unique_ptr<MipLevel> newLevel = std::make_unique<MipLevel>(width, heightPot, numChannels);

			for (int y = 0; y < heightPot; y++) {
				for (int x = 0; x < width; x++) {
					float sy = (float)(y * height) / heightPot;
					const int A = 3;
					for (int i = 0; i < numChannels; i++) {
						newLevel->at(x, y, i) = 0;
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
							float c = (yy >= 0 && yy < height) ? level->at(x, yy, i) : 0;
							newLevel->at(x, y, i) += c * f;
						}
					}
				}
			}
			level = std::move(newLevel);
		}

		return level;
	}

	void TextureBase::generateMipMaps()
	{
		int numChannels = mMipMaps[0]->numChannels();

		while (mMipMaps[mMipMaps.size() - 1]->width() > 1 || mMipMaps[mMipMaps.size() - 1]->height() > 1) {
			MipLevel &lastLevel = *mMipMaps[mMipMaps.size() - 1];

			int width = std::max(1, lastLevel.width() / 2);
			int height = std::max(1, lastLevel.height() / 2);
			std::unique_ptr<MipLevel> level = std::make_unique<MipLevel>(width, height, numChannels);

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int x0 = x * 2;
					int x1 = x0 + 1;
					int y0 = y * 2;
					int y1 = y0 + 1;

					for (int i = 0; i < numChannels; i++) {
						float s = lastLevel.at(x0, y0, i) + lastLevel.at(x1, y0, i) + lastLevel.at(x0, y1, i) + lastLevel.at(x1, y1, i);
						level->at(x, y, i) = s / 4.0f;
					}
				}
			}

			mMipMaps.push_back(std::move(level));
		}
	}

	TextureBase::MipLevel::MipLevel(int width, int height, int numChannels)
		: mWidth(width), mHeight(height), mNumChannels(numChannels), mValues(width * height * numChannels)
	{
	}

	TextureBase::MipLevel::MipLevel(int width, int height, int numChannels, std::vector<float> &&values)
		: mWidth(width), mHeight(height), mNumChannels(numChannels), mValues(std::move(values))
	{
	}

	int TextureBase::MipLevel::width() const
	{
		return mWidth;
	}

	int TextureBase::MipLevel::height() const
	{
		return mHeight;
	}

	int TextureBase::MipLevel::numChannels() const
	{
		return mNumChannels;
	}

	float &TextureBase::MipLevel::at(int x, int y, int channel)
	{
		return mValues[(y * mWidth + x) * mNumChannels + channel];
	}
}