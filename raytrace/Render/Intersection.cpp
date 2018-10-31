#include "Render/Intersection.hpp"

namespace Render {
	Intersection::Intersection()
		: mPrimitive(*(Object::Primitive*)0), mBeam(*(Render::Beam*)0), mDistance(FLT_MAX)
	{
	}

	Intersection::Intersection(const Object::Primitive &primitive, const Render::Beam &beam, const Math::Point &point, float distance, const Math::Normal &normal, const Object::Color &albedo)
		: mPrimitive(primitive), mBeam(beam), mPoint(point), mNormal(normal), mDistance(distance), mAlbedo(albedo)
	{
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
		return mDistance != FLT_MAX;
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
		return mDistance;
	}

	const Object::Color &Intersection::albedo() const
	{
		return mAlbedo;
	}
}