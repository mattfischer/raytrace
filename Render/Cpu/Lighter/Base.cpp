#include "Render/Cpu/Lighter/Base.hpp"

namespace Render::Cpu::Lighter {
    Base::Base()
    {
    }

    Base::~Base()
    {
    }

    std::vector<std::unique_ptr<Render::Cpu::Executor::Job>> Base::createPrerenderJobs(const Object::Scene &, Render::Framebuffer &)
    {
        return std::vector<std::unique_ptr<Render::Cpu::Executor::Job>>();
    }
}