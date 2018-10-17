#ifndef OBJECT_SHAPE_TRANSFORMED_HPP
#define OBJECT_SHAPE_TRANSFORMED_HPP

#include "Object/Shape/Base.hpp"

#include "Math/Transformation.hpp"

#include <memory>

namespace Object {
	namespace Shape {
		class Transformed : public Base
		{
		public:
			Transformed(std::unique_ptr<Base> shape, const Math::Transformation &transformation);

			virtual bool intersect(const Math::Ray &ray, Intersection &intersection) const;
			virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const;
			virtual bool sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const;

		private:
			std::unique_ptr<Base> mShape;
			Math::Transformation mTransformation;
		};
	}
}

#endif