#define _USE_MATH_DEFINES
#include "Object/Impl/Brdf/Lambert.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"
#include "Math/OrthonormalBasis.hpp"

#include <cmath>
#include <algorithm>

namespace Object::Impl::Brdf {
    Lambert::Lambert(float strength)
    {
        mStrength = strength;
    }

    float Lambert::lambert() const
    {
        return mStrength;
    }

    Math::Color Lambert::reflected(const Math::Vector &, const Math::Normal &, const Math::Vector &, const Math::Color &albedo) const
    {
        return albedo * mStrength / (float)M_PI;
    }

    Math::Vector Lambert::sample(Math::Sampler &sampler, const Math::Normal &nrm, const Math::Vector &) const
    {
        Math::Point2D samplePoint = sampler.getValue2D();
        Math::OrthonormalBasis basis(nrm);
        float phi = 2 * M_PI * samplePoint.u();
        float theta = std::asin(std::sqrt(samplePoint.v()));

        Math::Vector dirIn = basis.localToWorld(Math::Vector::fromPolar(phi, M_PI / 2 - theta, 1));

        return dirIn;
    }

    Math::Pdf Lambert::pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &) const
    {
        float cosTheta = std::max(dirIn * Math::Vector(nrm), 0.0f);
        float pdf = cosTheta / M_PI;

        return pdf;
    }

    void Lambert::writeProxy(BrdfProxy &proxy) const
    {
        proxy.type = BrdfProxy::Type::Lambert;
        proxy.lambert.strength = mStrength;
    }
}