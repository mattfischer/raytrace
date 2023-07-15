#define _USE_MATH_DEFINES
#include "Object/Texture.hpp"

#include <algorithm>
#include <cmath>

namespace Object {
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

        if (pot != x) {
            pot *= 2;
        }

        return pot;
    }

    TextureBase::TextureBase(int width, int height, int numChannels, std::vector<float> &&values)
    {
        std::unique_ptr<MipMap> baseMap = std::make_unique<MipMap>(width, height, numChannels, std::move(values));

        int widthPot = nextPowerOfTwo(width);
        int heightPot = nextPowerOfTwo(height);
        if (widthPot != width) {
            baseMap = resampleMipMap(*baseMap, widthPot, true);
        }

        if(heightPot != height) {
            baseMap = resampleMipMap(*baseMap, heightPot, false);
        }

        mMipMaps.push_back(std::move(baseMap));
    }

    const TextureBase::MipMap &TextureBase::mipMap(int level) const
    {
        return *mMipMaps[level];
    }

    void TextureBase::writeProxy(TextureProxy &proxy, OpenCL::Allocator &clAllocator) const
    {
        proxy.numMipMaps = mMipMaps.size();
        proxy.mipMaps = clAllocator.allocateArray<MipMapProxy>(proxy.numMipMaps);
        for(int i=0; i<mMipMaps.size(); i++) {
            mMipMaps[i]->writeProxy(proxy.mipMaps[i], clAllocator);
        }
    }

    void TextureBase::doSample(const Math::Point2D &samplePoint, const Math::Bivector2D &sampleProjection, float values[]) const
    {
        float lf = selectMipMap(sampleProjection);
        int l = (int)std::floor(lf);
        const std::unique_ptr<MipMap> &level0 = mMipMaps[l];
        const std::unique_ptr<MipMap> &level1 = mMipMaps[std::min(l + 1, (int)mMipMaps.size() - 1)];
        float dl = lf - l;

        for (int i = 0; i < level0->numChannels(); i++) {
            values[i] = 0;
        }

        level0->sample(samplePoint, 1 - dl, values);
        level1->sample(samplePoint, dl, values);
    }

    void TextureBase::generateMipMaps()
    {
        if (mMipMaps.size() > 1) {
            return;
        }

        int numChannels = mMipMaps[0]->numChannels();

        while (mMipMaps[mMipMaps.size() - 1]->width() > 1 || mMipMaps[mMipMaps.size() - 1]->height() > 1) {
            MipMap &lastLevel = *mMipMaps[mMipMaps.size() - 1];

            int width = std::max(1, lastLevel.width() / 2);
            int height = std::max(1, lastLevel.height() / 2);
            std::unique_ptr<MipMap> level = std::make_unique<MipMap>(width, height, numChannels);

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

    float TextureBase::selectMipMap(const Math::Bivector2D &sampleProjection) const
    {
        float projectionSize = std::sqrt(std::min(sampleProjection.u().magnitude2(), sampleProjection.v().magnitude2()));

        float level = mMipMaps.size() - 1 + std::log2f(projectionSize);
        return std::max(0.0f, std::min(level, (float)mMipMaps.size() - 1));
    }

    std::unique_ptr<TextureBase::MipMap> TextureBase::resampleMipMap(const MipMap &mipMap, int newSize, bool horizontal) const
    {
        int newWidth, newHeight;
        int fixedSize, oldSize;

        if (horizontal) {
            newWidth = newSize;
            newHeight = fixedSize = mipMap.height();
            oldSize = mipMap.width();
        }
        else {
            newWidth = fixedSize = mipMap.width();
            newHeight = newSize;
            oldSize = mipMap.height();
        }

        std::unique_ptr<MipMap> newMipMap = std::make_unique<MipMap>(newWidth, newHeight, mipMap.numChannels());

        for (int j = 0; j < fixedSize; j++) {
            for (int i = 0; i < newSize; i++) {
                int x = horizontal ? i : j;
                int y = horizontal ? j : i;

                float si = (float)(i * oldSize) / newSize;
                const int A = 3;
                for (int c = 0; c < newMipMap->numChannels(); c++) {
                    newMipMap->at(x, y, c) = 0;
                }

                for (int ii = (int)std::ceil(si) - A; ii <= (int)std::floor(si) + A; ii++) {
                    float u = si - ii;
                    float lanczos = 0;
                    if (u == 0) {
                        lanczos = 1;
                    }
                    else if (u >= -A && u <= A) {
                        lanczos = A * std::sin(M_PI * u) * std::sin(M_PI * u / A) / (M_PI * M_PI * u * u);
                    }
                    for (int c = 0; c < newMipMap->numChannels(); c++) {
                        if (ii >= 0 && ii < oldSize) {
                            float sample = horizontal ? mipMap.at(ii, y, c) : mipMap.at(x, ii, c);
                            newMipMap->at(x, y, c) += lanczos * sample;
                        }
                    }
                }
            }
        }

        return newMipMap;
    }

    TextureBase::MipMap::MipMap(int width, int height, int numChannels)
        : mWidth(width), mHeight(height), mNumChannels(numChannels), mValues(width * height * numChannels)
    {
    }

    TextureBase::MipMap::MipMap(int width, int height, int numChannels, std::vector<float> &&values)
        : mWidth(width), mHeight(height), mNumChannels(numChannels), mValues(std::move(values))
    {
    }

    int TextureBase::MipMap::width() const
    {
        return mWidth;
    }

    int TextureBase::MipMap::height() const
    {
        return mHeight;
    }

    int TextureBase::MipMap::numChannels() const
    {
        return mNumChannels;
    }

    float &TextureBase::MipMap::at(int x, int y, int channel)
    {
        return mValues[(y * mWidth + x) * mNumChannels + channel];
    }

    float TextureBase::MipMap::at(int x, int y, int channel) const
    {
        return mValues[(y * mWidth + x) * mNumChannels + channel];
    }

    void TextureBase::MipMap::sample(const Math::Point2D &samplePoint, float weight, float values[]) const
    {
        float fx = samplePoint.u() * mWidth;
        float fy = samplePoint.v() * mHeight;
        int x = (int)std::floor(fx);
        int y = (int)std::floor(fy);
        int x1 = (x == mWidth - 1) ? 0 : x + 1;
        int y1 = (y == mHeight - 1) ? 0 : y + 1;
        float dx = fx - x;
        float dy = fy - y;

        for (int i = 0; i < mNumChannels; i++) {
            float v = (1 - dx) * (1 - dy) * at(x, y, i) +
                dx * (1 - dy) * at(x1, y, i) +
                (1 - dx) * dy * at(x, y1, i) +
                dx * dy * at(x1, y1, i);
            values[i] += weight * v;
        }
    }

    void TextureBase::MipMap::writeProxy(MipMapProxy &proxy, OpenCL::Allocator &clAllocator) const
    {
        proxy.width = mWidth;
        proxy.height = mHeight;
        proxy.numChannels = mNumChannels;
        proxy.values = clAllocator.allocateArray<float>(mValues.size());
        memcpy(proxy.values, &mValues[0], mValues.size() * sizeof(float));
    }
}
