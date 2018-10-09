#include "Lighter/Base.hpp"

namespace Lighter {

Base::Base()
{
}

bool Base::prerender(const Object::Intersection &intersection, Render::Tracer &tracer)
{
	return false;
}

const Base::Probe &Base::probe() const
{
	return *mProbe;
}

void Base::clearProbe() const
{
	if (mProbe) {
		mProbe->entries.clear();
	}
}

void Base::addProbeEntry(const Math::Vector &direction, const Object::Radiance &radiance) const
{
	if (mProbe) {
		Probe::Entry entry;

		entry.direction = direction;
		entry.radiance = radiance;

		mProbe->entries.push_back(entry);
	}
}

void Base::enableProbe(bool enabled)
{
	if (enabled) {
		mProbe = std::make_unique<Probe>();
	}
	else {
		mProbe.reset();
	}
}

}