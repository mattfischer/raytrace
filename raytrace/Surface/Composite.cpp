#include "Surface/Composite.hpp"

namespace Surface {

Composite::Composite(Surface::Base **surfaces, int numSurfaces)
{
	mSurfaces = surfaces;
	mNumSurfaces = numSurfaces;
}

Composite::~Composite()
{
	if(mSurfaces) {
		for(int i=0; i<mNumSurfaces; i++) {
			delete mSurfaces[i];
		}

		delete[] mSurfaces;
	}
}

Object::Color Composite::color(const Trace::Intersection &intersection, const Trace::Tracer &tracer) const
{
	Object::Color totalColor;

	for(int i=0; i<mNumSurfaces; i++) {
		totalColor += mSurfaces[i]->color(intersection, tracer);
	}

	return totalColor / mNumSurfaces;
}

}