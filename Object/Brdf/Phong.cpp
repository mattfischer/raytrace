#define _USE_MATH_DEFINES
#include "Object/Brdf/Phong.hpp"

#include "Math/Normal.hpp"
#include "Math/Vector.hpp"
#include "Math/OrthonormalBasis.hpp"

#include <cmath>
#include <algorithm>

namespace Object::Brdf {
    Phong::Phong(float strength, float power)
    {
        mStrength = strength;
        mPower = power;
    }

    Math::Color Phong::reflected(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut, const Math::Color &) const
    {
        Math::Vector dirReflect = -(dirIn - Math::Vector(nrm) * (2 * (nrm * dirIn)));

        float dot = dirReflect * dirOut;
        float coeff = 0;
        if(dot > 0) {
            coeff = std::pow(dot, mPower);
        }

        return Math::Color(1, 1, 1) * mStrength * coeff * (mPower + 1) / (2 * (float)M_PI);
    }

    Math::Color Phong::transmitted(const Math::Vector &, const Math::Normal &, const Math::Color &) const
    {
        return Math::Color(1, 1, 1) * (1.0f - mStrength);
    }

    Math::Vector Phong::sample(Math::Sampler::Base &sampler, const Math::Normal &nrm, const Math::Vector &dirOut) const
    {
        Math::Point2D samplePoint = sampler.getValue2D();
        float phi = 2 * M_PI * samplePoint.u();
        float theta = std::acos(std::pow(samplePoint.v(), 1.0f / (mPower + 1)));

        Math::OrthonormalBasis basis(dirOut);

        Math::Vector dirReflect = basis.localToWorld(Math::Vector::fromPolar(phi, M_PI / 2 - theta, 1));
        Math::Vector dirIn = -(dirReflect - Math::Vector(nrm) * (dirReflect * nrm * 2));

        return dirIn;
    }

    float Phong::pdf(const Math::Vector &dirIn, const Math::Normal &nrm, const Math::Vector &dirOut) const
    {
        float coeff = 0;
        Math::Vector dirReflect = -(dirIn - Math::Vector(nrm) * (dirIn * nrm * 2));
        float dot = dirReflect * dirOut;
        if (dot > 0) {
            coeff = std::pow(dot, mPower);
        }

        float pdf = coeff * (mPower + 1) / (2 * M_PI);
        return std::min(pdf, 1000.0f);
    }

    void Phong::writeProxy(BrdfProxy &proxy) const
    {
        proxy.type = BrdfProxy::Type::Phong;
        proxy.phong.strength = mStrength;
        proxy.phong.power = mPower;
    }
}
