#ifndef LIGHTER_BASE_HPP
#define LIGHTER_BASE_HPP

#include "Render/Job.hpp"
#include "Render/Framebuffer.hpp"

#include "Math/Vector.hpp"

#include "Object/Radiance.hpp"
#include "Object/Intersection.hpp"

#include <vector>
#include <memory>

namespace Lighter {
	class Base {
	public:
		Base();

		struct Probe {
			struct Entry {
				Math::Vector direction;
				Object::Radiance radiance;
			};

			std::vector<Entry> entries;
		};

		virtual Object::Radiance light(const Object::Intersection &intersection, Render::Sampler &sampler, int generation) const = 0;
		virtual std::vector<std::unique_ptr<Render::Job>> createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer);

		void enableProbe(bool enabled);
		const Probe &probe() const;

	protected:
		void clearProbe() const;
		void addProbeEntry(const Math::Vector &direction, const Object::Radiance &radiance) const;

	private:
		std::unique_ptr<Probe> mProbe;
	};
}

#endif