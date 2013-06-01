#ifndef SURFACE_COMPOSITE_HPP
#define SURFACE_COMPOSITE_HPP

#include "Surface/Base.hpp"

namespace Surface {

class Composite : public Base
{
public:
	Composite(Base **surfaces, int numSurfaces);
	virtual ~Composite();

	virtual Object::Color color(const Trace::Intersection &intersection, const Trace::Tracer &tracer) const;

private:
	Base **mSurfaces;
	int mNumSurfaces;
};

}

#endif