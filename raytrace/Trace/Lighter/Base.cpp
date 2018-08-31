#include "Trace/Lighter/Base.hpp"

namespace Trace {
namespace Lighter {

Base::Base()
{
	mProbe = 0;
}

bool Base::prerender(const Trace::Intersection &intersection, Trace::Tracer &tracer) const
{
	return false;
}

void Base::setProbe(std::vector<ProbeEntry> *probe)
{
	mProbe = probe;
}

void Base::addProbeEntry(const Math::Vector &direction, const Object::Radiance &radiance) const
{
	if (mProbe) {
		mProbe->push_back(ProbeEntry{ direction, radiance });
	}
}

}
}