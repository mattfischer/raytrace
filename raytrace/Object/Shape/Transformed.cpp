#include "Object/Shape/Transformed.hpp"

namespace Object {
	namespace Shape {
		Transformed::Transformed(std::unique_ptr<Base> shape, const Math::Transformation &transformation)
			: mShape(std::move(shape)), mTransformation(transformation)
		{
		}

		bool Transformed::intersect(const Math::Ray &ray, Intersection &intersection) const
		{
			Math::Ray transformedRay = mTransformation.inverse() * ray;
			if (mShape->intersect(transformedRay, intersection)) {
				intersection.normal = (mTransformation * intersection.normal).normalize();
				return true;
			}

			return false;
		}

		BoundingVolume Transformed::boundingVolume(const Math::Transformation &transformation) const
		{
			return mShape->boundingVolume(transformation * mTransformation);
		}
	}
}