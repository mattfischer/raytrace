#include "Render/Intersection.hpp"

namespace Render {
	Intersection::Intersection()
		: mPrimitive(*(Object::Primitive*)0), mBeam(*(Render::Beam*)0)
	{
		mShapeIntersection.distance = FLT_MAX;
	}

	Intersection::Intersection(const Object::Primitive &primitive, const Render::Beam &beam, const Object::Shape::Base::Intersection &shapeIntersection)
		: mPrimitive(primitive), mBeam(beam), mShapeIntersection(shapeIntersection)
	{
		mPoint = mBeam.ray().origin() + mBeam.ray().direction() * mShapeIntersection.distance;
		Math::Bivector projection = mBeam.project(mShapeIntersection.distance, mShapeIntersection.normal);
		Math::Vector v = mShapeIntersection.tangent.u() % mShapeIntersection.tangent.v();
		v = v / v.magnitude2();
		Math::Vector2D du((projection.u() % mShapeIntersection.tangent.v()) * v, (mShapeIntersection.tangent.u() % projection.u()) * v);
		Math::Vector2D dv((projection.v() % mShapeIntersection.tangent.v()) * v, (mShapeIntersection.tangent.u() % projection.v()) * v);
		Math::Bivector2D surfaceProjection(du, dv);

		mAlbedo = mPrimitive.surface().albedo().color(mShapeIntersection.surfacePoint, surfaceProjection);
		mNormal = mShapeIntersection.normal;
		if (mPrimitive.surface().hasNormalMap()) {
			mNormal = mPrimitive.surface().normalMap().perturbNormal(mShapeIntersection.surfacePoint, surfaceProjection, mNormal, shapeIntersection.tangent);
		}
	};

	const Math::Normal &Intersection::normal() const
	{
		return mNormal;
	}

	const Math::Point &Intersection::point() const
	{
		return mPoint;
	}

	bool Intersection::valid() const
	{
		return mShapeIntersection.distance != FLT_MAX;
	}

	const Object::Primitive &Intersection::primitive() const
	{
		return mPrimitive;
	}

	const Math::Ray &Intersection::ray() const
	{
		return mBeam.ray();
	}

	const Render::Beam &Intersection::beam() const
	{
		return mBeam;
	}

	float Intersection::distance() const
	{
		return mShapeIntersection.distance;
	}

	const Object::Color &Intersection::albedo() const
	{
		return mAlbedo;
	}
}