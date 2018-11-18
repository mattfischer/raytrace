#include "Object/Intersection.hpp"

namespace Object {
	Intersection::Intersection()
		: mScene(*(Object::Scene*)0), mPrimitive(*(Object::Primitive*)0), mBeam(*(Math::Beam*)0)
	{
		mShapeIntersection.distance = FLT_MAX;
	}

	Intersection::Intersection(const Object::Scene &scene, const Object::Primitive &primitive, const Math::Beam &beam, const Object::Shape::Base::Intersection &shapeIntersection)
		: mScene(scene), mPrimitive(primitive), mBeam(beam), mShapeIntersection(shapeIntersection)
	{
		mPoint = mBeam.ray().origin() + mBeam.ray().direction() * mShapeIntersection.distance;

		mAlbedoValid = false;
		mNormalValid = false;
		mSurfaceProjectionValid = false;
	};

	const Math::Normal &Intersection::normal() const
	{
		if (!mNormalValid) {
			mNormal = mShapeIntersection.normal;
			if (mPrimitive.surface().hasNormalMap()) {
				mNormal = mPrimitive.surface().normalMap().perturbNormal(mShapeIntersection.surfacePoint, surfaceProjection(), mNormal, mShapeIntersection.tangent);
			}
			Math::Vector outgoingDirection = -mBeam.ray().direction();
			float dot = mNormal * outgoingDirection;
			mFacingNormal = (dot > 0) ? mNormal : -mNormal;
			mNormalValid = true;
		}

		return mNormal;
	}

	const Math::Normal &Intersection::facingNormal() const
	{
		normal();
		return mFacingNormal;
	}

	const Math::Point &Intersection::point() const
	{
		return mPoint;
	}

	bool Intersection::valid() const
	{
		return mShapeIntersection.distance != FLT_MAX;
	}

	const Object::Scene &Intersection::scene() const
	{
		return mScene;
	}

	const Object::Primitive &Intersection::primitive() const
	{
		return mPrimitive;
	}

	const Math::Ray &Intersection::ray() const
	{
		return mBeam.ray();
	}

	const Math::Beam &Intersection::beam() const
	{
		return mBeam;
	}

	float Intersection::distance() const
	{
		return mShapeIntersection.distance;
	}

	const Object::Color &Intersection::albedo() const
	{
		if (!mAlbedoValid) {
			mAlbedo = mPrimitive.surface().albedo().color(mShapeIntersection.surfacePoint, surfaceProjection());
			mAlbedoValid = true;
		}

		return mAlbedo;
	}

	const Math::Bivector2D &Intersection::surfaceProjection() const
	{
		if (!mSurfaceProjectionValid) {
			Math::Bivector projection = mBeam.project(mShapeIntersection.distance, mShapeIntersection.normal);
			Math::Vector v = mShapeIntersection.tangent.u() % mShapeIntersection.tangent.v();
			v = v / v.magnitude2();
			Math::Vector2D du((projection.u() % mShapeIntersection.tangent.v()) * v, (mShapeIntersection.tangent.u() % projection.u()) * v);
			Math::Vector2D dv((projection.v() % mShapeIntersection.tangent.v()) * v, (mShapeIntersection.tangent.u() % projection.v()) * v);
			mSurfaceProjection = Math::Bivector2D(du, dv);
			mSurfaceProjectionValid = true;
		}

		return mSurfaceProjection;
	}
}