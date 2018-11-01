#ifndef OBJECT_TEXTURE_HPP
#define OBJECT_TEXTURE_HPP

#include "Object/Color.hpp"
#include "Math/Point2D.hpp"
#include "Math/Vector2D.hpp"
#include "Math/Bivector2D.hpp"

#include <vector>

namespace Object {
	template <int NUM_CHANNELS>
	class Texture
	{
	public:
		Texture::Texture(int width, int height, std::vector<float> &&values)
			: mWidth(width), mHeight(height), mValues(std::move(values))
		{
		}

		struct Value {
			float channels[NUM_CHANNELS];
		};

		Value sample(const Math::Point2D &samplePoint, const Math::Bivector2D &sampleProjection) const
		{
			int x = samplePoint.u() * mWidth;
			int y = samplePoint.v() * mHeight;

			Value value;
			for (int i = 0; i < NUM_CHANNELS; i++) {
				value.channels[i] = mValues[(y * mWidth + x) * NUM_CHANNELS + i];
			}

			return value;
		}

		void gradient(const Math::Point2D &samplePoint, Math::Vector2D gradient[NUM_CHANNELS]) const
		{
			int x = samplePoint.u() * mWidth;
			int y = samplePoint.v() * mHeight;

			for (int i = 0; i < NUM_CHANNELS; i++) {
				float du = (mValues[(y * mWidth + x + 1) * NUM_CHANNELS + i] - mValues[(y * mWidth + x) * NUM_CHANNELS + i]) * mWidth;
				float dv = (mValues[((y + 1) * mWidth + x) * NUM_CHANNELS + i] - mValues[(y * mWidth + x) * NUM_CHANNELS + i]) * mHeight;
				gradient[i] = Math::Vector2D(du, dv);
			}
		}

	protected:
		std::vector<float> mValues;
		int mWidth;
		int mHeight;
	};
}
#endif