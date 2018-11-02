#ifndef OBJECT_TEXTURE_HPP
#define OBJECT_TEXTURE_HPP

#include "Object/Color.hpp"
#include "Math/Point2D.hpp"
#include "Math/Vector2D.hpp"
#include "Math/Bivector2D.hpp"

#include <vector>
#include <memory>

namespace Object {
	class TextureBase
	{
	public:
		TextureBase(int width, int height, int numChannels, std::vector<float> &&values);

	protected:
		class MipLevel {
		public:
			MipLevel(int width, int height, int numChannels);
			MipLevel(int width, int height, int numChannels, std::vector<float> &&values);

			int width() const;
			int height() const;
			int numChannels() const;

			float &at(int x, int y, int channel);

		public:
			int mWidth;
			int mHeight;
			int mNumChannels;
			std::vector<float> mValues;
		};

		void doSample(const Math::Point2D &samplePoint, const Math::Bivector2D &sampleProjection, float values[]) const;
		void doGradient(const Math::Point2D &samplePoint, const Math::Bivector2D &sampleProjection, Math::Vector2D gradient[]) const;

		std::unique_ptr<MipLevel> createBaseLevel(int width, int height, int numChannels, std::vector<float> &&values);
		void generateMipMaps();
		int selectMipLevel(const Math::Bivector2D &sampleProjection) const;

		std::vector<std::unique_ptr<MipLevel>> mMipMaps;
	};

	template <int NUM_CHANNELS>
	class Texture : public TextureBase
	{
	public:
		Texture(int width, int height, std::vector<float> &&values)
			: TextureBase(width, height, NUM_CHANNELS, std::move(values))
		{
		}

		struct Value {
			float channels[NUM_CHANNELS];
		};

		Value sample(const Math::Point2D &samplePoint, const Math::Bivector2D &sampleProjection) const
		{
			Value value;
			doSample(samplePoint, sampleProjection, value.channels);

			return value;
		}

		void gradient(const Math::Point2D &samplePoint, const Math::Bivector2D &sampleProjection, Math::Vector2D gradient[NUM_CHANNELS]) const
		{
			doGradient(samplePoint, sampleProjection, gradient);
		}
	};
}
#endif