#ifndef RENDER_SETTINGS_HPP
#define RENDER_SETTINGS_HPP

namespace Render {
    struct Settings
    {
        unsigned int width;
        unsigned int height;
        unsigned int minSamples;
        unsigned int maxSamples;
        float sampleThreshold;
    };
}
#endif
