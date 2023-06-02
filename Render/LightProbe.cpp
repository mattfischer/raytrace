#include "Render/LightProbe.hpp"

#include "Render/Renderer.hpp"

namespace Render {
    LightProbe::LightProbe(const Object::Intersection &isect)
    : mIntersection(isect)
    , mBasis(isect.primitive().surface().facingNormal(isect))      
    {
    }

    void LightProbe::getSample(float &azimuth, float &elevation, Object::Color &color)
    {
        const Object::Surface &surface = mIntersection.primitive().surface();
        const Math::Normal &nrmFacing = surface.facingNormal(mIntersection);
        const Math::Vector dirOut = -mIntersection.ray().direction();
        
        float pdf;
        bool pdfDelta;
        Math::Vector dirIn;
        surface.sample(mIntersection, mSampler, dirIn, pdf, pdfDelta);
        float dot = dirIn * nrmFacing;
        Math::Point pntOffset = mIntersection.point() + Math::Vector(nrmFacing) * 0.01f;
		Math::Ray ray(pntOffset, dirIn);
		Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
		
		Object::Intersection isect2 = mIntersection.scene().intersect(beam);
        Object::Radiance irad;
		if (isect2.valid()) {
			irad = mLighter.light(isect2, mSampler) * dot;
		}

        Math::Vector dirInLocal = mBasis.worldToLocal(dirIn);
        azimuth = std::atan2(dirInLocal.y(), dirInLocal.x());
        elevation = std::asin(dirInLocal.z());
        color = Framebuffer::toneMap(irad);
    }

}