#ifndef OBJECT_TEXTURE_HPP
#define OBJECT_TEXTURE_HPP

#include "Math/Color.hpp"
#include "Math/Point2D.hpp"
#include "Math/Vector2D.hpp"
#include "Math/Bivector2D.hpp"

#include <vector>
#include <memory>

#include "Object/CLProxies.hpp"
#include "OpenCL.hpp"

namespace Object {
    class TextureBase
    {
    public:
        class MipMap {
        public:
            MipMap(int width, int height, int numChannels);
            MipMap(int width, int height, int numChannels, std::vector<float> &&values);

            int width() const;
            int height() const;
            int numChannels() const;

            float &at(int x, int y, int channel);
            float at(int x, int y, int channel) const;

            void sample(const Math::Point2D &samplePoint, float weight, float values[]) const;

            void writeProxy(MipMapProxy &proxy, OpenCL::Allocator &clAllocator) const;

        private:
            int mWidth;
            int mHeight;
            int mNumChannels;
            std::vector<float> mValues;
        };

        TextureBase(int width, int height, int numChannels, std::vector<float> &&values);

        void generateMipMaps();
        const MipMap &mipMap(int level) const;

        void writeProxy(TextureProxy &proxy, OpenCL::Allocator &clAllocator) const;

    protected:
        void doSample(const Math::Point2D &samplePoint, const Math::Bivector2D &sampleProjection, float values[]) const;

    private:
        float selectMipMap(const Math::Bivector2D &sampleProjection) const;
        std::unique_ptr<MipMap> resampleMipMap(const MipMap &mipMap, int newSize, bool horizontal) const;

        std::vector<std::unique_ptr<MipMap>> mMipMaps;
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
    };
}
#endif