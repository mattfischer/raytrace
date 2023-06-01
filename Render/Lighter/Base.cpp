#include "Render/Lighter/Base.hpp"

namespace Render {
    namespace Lighter {
        Base::Base()
        {
        }

        Base::~Base()
        {
        }

        std::vector<std::unique_ptr<Render::Executor::Job>> Base::createPrerenderJobs(const Object::Scene &, Render::Framebuffer &)
        {
            return std::vector<std::unique_ptr<Render::Executor::Job>>();
        }
    }
}