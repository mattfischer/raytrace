#include "Object/Brdf.hpp"

#include <vector>

namespace Object {
    Brdf::~Brdf()
    {
    }

    Math::Color Brdf::reflected(const Math::Vector &, const Math::Normal &, const Math::Vector &, const Math::Color &) const
    {
        return Math::Color(0, 0, 0);
    }

    Math::Color Brdf::transmitted(const Math::Vector &, const Math::Normal &, const Math::Color &) const
    {
        return Math::Color(0, 0, 0);
    }

    float Brdf::lambert() const
    {
        return 0;
    }

    Math::Vector Brdf::sample(Math::Sampler &, const Math::Normal &, const Math::Vector &) const
    {
        return Math::Vector(0, 0, 0);
    }

    float Brdf::pdf(const Math::Vector &, const Math::Normal &, const Math::Vector &) const
    {
        return 0;
    }

    bool Brdf::opaque() const
    {
        return true;
    }

    float Brdf::transmitIor() const
    {
        return 1;
    }
}
