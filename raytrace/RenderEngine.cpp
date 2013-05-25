#include "RenderEngine.hpp"

#include "Object/Color.hpp"

RenderEngine::RenderEngine(Object::Scene *scene, const Trace::Tracer::Settings &settings)
{
	mScene = scene;
	mSettings = settings;
}

void RenderEngine::render(unsigned char *bits, LineCallback callback, void *data)
{
	Trace::Tracer tracer(mScene, mSettings);

	for(int y=0; y<mSettings.height; y++)
	{
		for(int x=0; x<mSettings.width; x++)
		{
			Object::Color c = tracer.tracePixel(x, y);

			bits[x*3] = c.blue() * 0xFF;
			bits[x*3 + 1] = c.green() * 0xFF;
			bits[x*3 + 2] = c.red() * 0xFF;
		}

		if(callback != 0) {
			callback(bits, y, data);
		}
	}
}