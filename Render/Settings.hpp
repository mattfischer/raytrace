#ifndef RENDER_SETTINGS_HPP
#define RENDER_SETTINGS_HPP

#include "Lighter/UniPath.hpp"

namespace Render {
	struct Settings
	{
		int width;
		int height;
		bool lighting;
		int minSamples;
		int maxSamples;
		float sampleThreshold;
        Lighter::UniPath::Settings lighterSettings;
	};
}
#endif
