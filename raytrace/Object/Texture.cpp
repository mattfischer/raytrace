#include "Object/Texture.hpp"

namespace Object {
	TextureBase::TextureBase(int width, int height, std::vector<float> &&values)
		: mWidth(width), mHeight(height), mValues(std::move(values))
	{
	}

	void TextureBase::doSample(const Math::Point2D &samplePoint, const Math::Bivector2D &sampleProjection, int numChannels, float values[]) const
	{
		int x = samplePoint.u() * mWidth;
		int y = samplePoint.v() * mHeight;

		for (int i = 0; i < numChannels; i++) {
			values[i] = mValues[(y * mWidth + x) * numChannels + i];
		}
	}

	void TextureBase::doGradient(const Math::Point2D &samplePoint, int numChannels, Math::Vector2D gradient[]) const
	{
		int x = samplePoint.u() * mWidth;
		int y = samplePoint.v() * mHeight;

		for (int i = 0; i < numChannels; i++) {
			float du = (mValues[(y * mWidth + x + 1) * numChannels + i] - mValues[(y * mWidth + x) * numChannels + i]) * mWidth;
			float dv = (mValues[((y + 1) * mWidth + x) * numChannels + i] - mValues[(y * mWidth + x) * numChannels + i]) * mHeight;
			gradient[i] = Math::Vector2D(du, dv);
		}
	}
}