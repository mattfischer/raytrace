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
        virtual ~Base();

        virtual Object::Radiance light(const Object::Intersection &intersection, Render::Sampler &sampler) const = 0;
		virtual std::vector<std::unique_ptr<Render::Job>> createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer);
	};
}

#endif
