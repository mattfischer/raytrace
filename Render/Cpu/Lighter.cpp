#include "Render/Cpu/Lighter.hpp"

namespace Render::Cpu {
    Lighter::Lighter()
    {
    }

    Lighter::~Lighter()
    {
    }

    std::vector<std::unique_ptr<Render::Cpu::Executor::Job>> Lighter::createPrerenderJobs(const Object::Scene &, Render::Framebuffer &)
    {
        return std::vector<std::unique_ptr<Render::Cpu::Executor::Job>>();
    }
}