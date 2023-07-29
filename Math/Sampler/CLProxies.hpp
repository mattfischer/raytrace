#ifndef MATH_SAMPLER_CLPROXIES_HPP
#define MATH_SAMPLER_CLPROXIES_HPP

#include <stdint.h>

struct SamplerProxy {
    unsigned int widthExponent;
    unsigned int widthAligned;
    unsigned int heightExponent;
    unsigned int heightAligned;
    unsigned int sampleStride;

    int euclidX;
    int euclidY;

    int numPrimes;
    int *primes;

    int *primeIndices;
    int *scrambledDigitsStart;
    int *scrambledDigits;
};

struct SamplerStateProxy {
    uint64_t index;
    unsigned int nextDimension;
};

#endif