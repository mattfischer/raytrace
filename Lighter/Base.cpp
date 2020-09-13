#include "Lighter/Base.hpp"

namespace Lighter {
	Base::Base()
	{
	}

    Base::~Base()
    {
    }

    std::vector<std::unique_ptr<Render::Job>> Base::createPrerenderJobs(const Object::Scene &, Render::Framebuffer &)
	{
		return std::vector<std::unique_ptr<Render::Job>>();
	}
}
