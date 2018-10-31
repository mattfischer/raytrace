#ifndef RENDER_INTERSECTION_HPP
#define RENDER_INTERSECTION_HPP

#include "Math/Point.hpp"
#include "Math/Vector.hpp"
#include "Math/Normal.hpp"
#include "Math/Transformation.hpp"

#include "Object/Primitive.hpp"

#include "Render/Beam.hpp"

#include <vector>

namespace Render {
	class Intersection
	{
	public:
		Intersection();
		Intersection(const Object::Primitive &primitive, const Render::Beam &beam, const Math::Point &point, float distance, const Math::Normal &normal, const Object::Color &albedo);

		bool valid() const;

		const Object::Primitive &primitive() const;

		float distance() const;
		const Math::Ray &ray() const;
		const Render::Beam &beam() const;
		const Math::Normal &normal() const;
		const Math::Point &point() const;
		const Object::Color &albedo() const;

	protected:
		const Object::Primitive &mPrimitive;
		const Render::Beam &mBeam;
		Math::Point mPoint;
		float mDistance;
		Math::Normal mNormal;
		Object::Color mAlbedo;
	};
}

#endif
