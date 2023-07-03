#ifndef PROXIES_HPP
#define PROXIES_HPP

#include <stdint.h>

#include "Math/CLProxies.hpp"
#include "Object/CLProxies.hpp"

struct SettingsProxy {
    int width;
    int height;
    int minSamples;
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
};

struct WorkQueueProxy {
    int *data;
};

struct ContextProxy {
    SceneProxy scene;
    SettingsProxy settings;
    ItemProxy *items;
    float *random;
    unsigned int currentPixel;

    WorkQueueProxy generateCameraRayQueue;
    WorkQueueProxy intersectRaysQueue;
    WorkQueueProxy directLightAreaQueue;
    WorkQueueProxy directLightPointQueue;
    WorkQueueProxy extendPathQueue;
    WorkQueueProxy commitRadianceQueue;
};

#endif