#ifndef PROXIES_HPP
#define PROXIES_HPP

#include <stdint.h>

#include "Math/CLProxies.hpp"
#include "Math/Impl/Sampler/CLProxies.hpp"
#include "Object/CLProxies.hpp"

struct SettingsProxy {
    int width;
    int height;
    int samples;
};

struct ItemProxy {
    BeamProxy beam;
    IntersectionProxy isect;
    bool specularBounce;
    int generation;
    float pdf;
    ColorProxy throughput;
    RadianceProxy radiance;
    int lightIndex;
    int x;
    int y;
    SamplerStateProxy samplerState;
};

struct WorkQueueProxy {
    int *data;
};

struct ContextProxy {
    SceneProxy scene;
    SettingsProxy settings;
    ItemProxy *items;
    unsigned int currentPixel;
    SamplerProxy sampler;

    WorkQueueProxy generateCameraRayQueue;
    WorkQueueProxy intersectRaysQueue;
    WorkQueueProxy directLightAreaQueue;
    WorkQueueProxy directLightPointQueue;
    WorkQueueProxy extendPathQueue;
    WorkQueueProxy commitRadianceQueue;
};

#endif