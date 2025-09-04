typedef struct {
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
} Sampler;

typedef struct {
    unsigned long index;
    unsigned int nextDimension;
} SamplerState;

void Sampler_startSample(Sampler *sampler, SamplerState *state, unsigned int x, unsigned int y, unsigned int sample)
{
    int xr = 0;
    for(int i=0; i<sampler->widthExponent; i++) {
        xr = 2 * xr + x % 2;
        x /= 2;
    }

    int yr = 0;
    for(int i=0; i<sampler->heightExponent; i++) {
        yr = 3 * yr + y % 3;
        y /= 3;
    }

    unsigned long idx = xr * sampler->euclidY * sampler->heightAligned + yr * sampler->euclidX * sampler->widthAligned;
    if(idx < 0) {
        idx = sampler->sampleStride - (-idx % sampler->sampleStride);
    }
    
    state->index = idx + sample * sampler->sampleStride;
    state->nextDimension = 0;
}

float Sampler_getValue(Sampler *sampler, SamplerState *state)
{
    int primeIndex = sampler->primeIndices[state->nextDimension];
    int b = sampler->primes[primeIndex];

    unsigned long N = 0;
    unsigned long D = 1;

    unsigned long x = state->index;
    int i = 0;
    int scrambledDigitsStart = sampler->scrambledDigitsStart[primeIndex];
    while(x > 0) {
        if((state->nextDimension == 0 && i < sampler->widthExponent) || (state->nextDimension == 1 && i < sampler->heightExponent)) {
            // ...
        } else {
            N = N * b + sampler->scrambledDigits[scrambledDigitsStart + x % b];
            D *= b; 
        }
        x /= b;
        i++;
    }

    float nd = (float)sampler->scrambledDigits[scrambledDigitsStart] / (float)(b - 1);
    float f = ((float)N + nd) / (float)D;        
    state->nextDimension++;

    if(state->nextDimension == sampler->numPrimes) {
        state->nextDimension = 0;
    }

    return f;
}

float2 Sampler_getValue2D(Sampler *sampler, SamplerState *state)
{
    return (float2)(Sampler_getValue(sampler, state), Sampler_getValue(sampler, state));
}