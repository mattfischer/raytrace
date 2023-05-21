#include "Math/Sampler/Halton.hpp"

#include <algorithm>
#include <tuple>

namespace Math {
    namespace Sampler {
        const int sPrimes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };

        Halton::Halton(unsigned int numDimensions)
        {
            mNumDimensions = std::min((size_t)numDimensions, sizeof(sPrimes) / sizeof(sPrimes[0]));
            mLastSamples.resize(mNumDimensions);
        }

        void Halton::startSequence(unsigned int index)
        {
            mCurrentDimension = 0;
            int dimension = 0;
            for (std::tuple<unsigned int, unsigned int> &sample : mLastSamples) {
                int B = sPrimes[dimension];
                int D = 1;
                int N = 0;
                int I = index;
                while (I > 0) {
                    D = D * B;
                    N = N * B + I % B;
                    I = I / B;
                }
                sample = std::make_tuple(N, D);
                dimension++;
            }
        }

        void Halton::startSequence(const Base::State &state)
        {
            const Halton::State &haltonState = static_cast<const Halton::State&>(state);

            mCurrentDimension = 0;
            mLastSamples = haltonState.lastSamples;
        }

        void Halton::startSample()
        {
            mCurrentDimension = 0;
        }

        float Halton::getValue()
        {
            if (mCurrentDimension >= mNumDimensions) {
                std::uniform_real_distribution<float> dist(0, 1);
                return dist(mRandomEngine);
            }

            int b = sPrimes[mCurrentDimension];

            int N;
            int D;
            std::tie(N, D) = mLastSamples[mCurrentDimension];

            int X = D - N;
            if (X == 1) {
                N = 1;
                D = b * D;
            }
            else {
                int Y = D / b;
                while (X <= Y) {
                    Y = Y / b;
                }
                N = (b + 1) * Y - X;
            }

            mLastSamples[mCurrentDimension] = std::make_tuple(N, D);
            mCurrentDimension++;
            return static_cast<float>(N) / static_cast<float>(D);
        }

        Math::Point2D Halton::getValue2D()
        {
            return Math::Point2D(getValue(), getValue());
        }

        void Halton::recordState(Base::State &state) const
        {
            Halton::State &haltonState = static_cast<Halton::State&>(state);

            haltonState.lastSamples = mLastSamples;
        }
    }
}