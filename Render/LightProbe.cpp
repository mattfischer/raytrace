#include "Render/LightProbe.hpp"

#include "Object/Scene.hpp"

namespace Render {
    LightProbe::LightProbe(const Object::Intersection &isect)
    : mIntersection(isect)
    , mBasis(isect.facingNormal())      
    {
    }

    void LightProbe::getSample(float &azimuth, float &elevation, Math::Color &color)
    {
        const Object::Surface &surface = mIntersection.primitive().surface();
        const Math::Normal &nrmFacing = mIntersection.facingNormal();
        const Math::Vector dirOut = -mIntersection.ray().direction();
        
        auto [_, dirIn, pdf] = surface.sample(mIntersection, mSampler);
        float dot = dirIn * nrmFacing;
        Math::Point pntOffset = mIntersection.point() + Math::Vector(nrmFacing) * 0.01f;
		Math::Ray ray(pntOffset, dirIn);
		Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
		
		Object::Intersection isect2 = mIntersection.scene().intersect(beam, FLT_MAX, true);
        Math::Radiance irad;
		if (isect2.valid()) {
			irad = mLighter.light(isect2, mSampler) * dot;
		}

        Math::Vector dirInLocal = mBasis.worldToLocal(dirIn);
        azimuth = std::atan2(dirInLocal.y(), dirInLocal.x());
        elevation = std::asin(dirInLocal.z());
        color = Framebuffer::toneMap(irad);
    }

}