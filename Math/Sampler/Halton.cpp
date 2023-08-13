#include "Math/Sampler/Halton.hpp"

#include <algorithm>
#include <tuple>
#include <mutex>
#include <random>
#include <vector>

namespace Math {
    namespace Sampler {
        const int sPrimes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 
                                53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109,
                                113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179,
                                181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241,
                                251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313,
                                317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389,
                                397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461,
                                463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547,
                                557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617,
                                619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691,
                                701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773,
                                787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859,
                                863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947,
                                953, 967, 971, 977, 983, 991, 997 };
        static const int sNumPrimes = sizeof(sPrimes) / sizeof(sPrimes[0]);

        std::vector<int> sPrimeIndices;
        std::vector<int> sScrambledDigitsStart;
        std::vector<int> sScrambledDigits;
        std::mutex sMutex;

        static int euclid(int a, int b, int &m, int &n)
        {
            int r0 = a; int r1 = b;
            int s0 = 1; int s1 = 0;
            int t0 = 0; int t1 = 1;

            while(r1 > 0) {
                int q = r0 / r1;
                int r2 = r0 - q * r1; r0 = r1; r1 = r2;
                int s2 = s0 - q * s1; s0 = s1; s1 = s2;
                int t2 = t0 - q * t1; t0 = t1; t1 = t2;
            }

            m = s0;
            n = t0;

            return r0;
        }

        Halton::Halton(int width, int height)
        {
            mIndex = 0;
            mNextDimension = 0;

            mWidthExponent = 0;
            mWidthAligned = 1;
            while(mWidthAligned < width) {
                mWidthExponent++;
                mWidthAligned *= 2;
            }

            mHeightExponent = 0;
            mHeightAligned = 1;
            while(mHeightAligned < height) {
                mHeightExponent++;
                mHeightAligned *= 3;
            }

            euclid(mWidthAligned, mHeightAligned, mEuclidX, mEuclidY);
            mSampleStride = mWidthAligned * mHeightAligned;
            
            std::lock_guard<std::mutex> lock(sMutex);
            if(sPrimeIndices.size() == 0) {                        
                std::default_random_engine randomEngine;
                randomEngine.seed(0x12345678);

                sPrimeIndices.resize(sNumPrimes);
                for(int i=0; i<sNumPrimes; i++) {
                    sPrimeIndices[i] = i;
                    std::vector<int> digits(sPrimes[i]);
                    for(int j=0; j<sPrimes[i]; j++) {
                        digits[j] = j;
                    }

                    if(i > 1) {
                        std::shuffle(digits.begin(), digits.end(), randomEngine);
                    }
                    sScrambledDigitsStart.push_back(sScrambledDigits.size());
                    sScrambledDigits.insert(sScrambledDigits.end(), digits.begin(), digits.end());
                }
                std::shuffle(sPrimeIndices.begin() + 2, sPrimeIndices.end(), randomEngine);
            }
        }

        void Halton::startSample(unsigned int index)
        {
            mIndex = index;
            mNextDimension = 0;
        }

        void Halton::startSample(unsigned int x, unsigned int y, unsigned int sample)
        {
            int xr = 0;
            for(int i=0; i<mWidthExponent; i++) {
                xr = 2 * xr + x % 2;
                x /= 2;
            }

            int yr = 0;
            for(int i=0; i<mHeightExponent; i++) {
                yr = 3 * yr + y % 3;
                y /= 3;
            }

            int64_t idx = xr * mEuclidY * mHeightAligned + yr * mEuclidX * mWidthAligned;
            if(idx < 0) {
                idx = mSampleStride - (-idx % mSampleStride);
            }
            
            mIndex = idx + sample * mSampleStride;
            mNextDimension = 0;
        }

        float Halton::getValue()
        {
            int primeIndex = sPrimeIndices[mNextDimension];
            int b = sPrimes[primeIndex];

            uint64_t N = 0;
            uint64_t D = 1;

            uint64_t x = mIndex;
            int i = 0;
            int scrambledDigitsStart = sScrambledDigitsStart[primeIndex];
            while(x > 0) {
                if(mNextDimension == 0 && i < mWidthExponent || mNextDimension == 1 && i < mHeightExponent) {
                    // ...
                } else {
                    N = N * b + sScrambledDigits[scrambledDigitsStart + x % b];
                    D *= b; 
                }
                x /= b;
                i++;
            }

            float nd = static_cast<float>(sScrambledDigits[scrambledDigitsStart]) / static_cast<float>(b - 1);
            float f = (static_cast<float>(N) + nd) / static_cast<float>(D);
            if(f == 1.0f) {
                f = 0.0f;
            }
            mNextDimension++;

            if(mNextDimension == sNumPrimes) {
                mNextDimension = 0;
            }

            return f;
        }

        void Halton::writeProxy(SamplerProxy &proxy, OpenCL::Allocator &clAllocator) const
        {
            proxy.widthExponent = mWidthExponent;
            proxy.widthAligned = mWidthAligned;
            proxy.heightExponent = mHeightExponent;
            proxy.heightAligned = mHeightAligned;
            proxy.sampleStride = mSampleStride;
            proxy.euclidX = mEuclidX;
            proxy.euclidY = mEuclidY;

            proxy.numPrimes = sNumPrimes;
            proxy.primes = clAllocator.allocateArray<int>(sNumPrimes);
            proxy.primeIndices = clAllocator.allocateArray<int>(sNumPrimes);
            for(int i=0; i<proxy.numPrimes; i++) {
                proxy.primes[i] = sPrimes[i];
                proxy.primeIndices[i] = sPrimeIndices[i];
            }

            proxy.scrambledDigitsStart = clAllocator.allocateArray<int>(sScrambledDigitsStart.size());
            for(int i=0; i<sScrambledDigitsStart.size(); i++) {
                proxy.scrambledDigitsStart[i] = sScrambledDigitsStart[i];
            }
            
            proxy.scrambledDigits = clAllocator.allocateArray<int>(sScrambledDigits.size());
            for(int i=0; i<sScrambledDigits.size(); i++) {
                proxy.scrambledDigits[i] = sScrambledDigits[i];
            }
        }
    }
}