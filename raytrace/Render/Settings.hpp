#ifndef RENDER_SETTINGS_HPP
#define RENDER_SETTINGS_HPP

#include "Lighter/Master.hpp"

namespace Render {
	struct Settings
	{
		int width;
		int height;
		bool lighting;
		int minSamples;
		int maxSamples;
		float sampleThreshold;
		Lighter::Master::Settings lighterSettings;
	};
}
#endif