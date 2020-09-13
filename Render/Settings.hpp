#ifndef RENDER_SETTINGS_HPP
#define RENDER_SETTINGS_HPP

#include "Lighter/UniPath.hpp"

namespace Render {
	struct Settings
	{
        unsigned int width;
        unsigned int height;
		bool lighting;
        unsigned int minSamples;
        unsigned int maxSamples;
		float sampleThreshold;
        Lighter::UniPath::Settings lighterSettings;
	};
}
#endif
