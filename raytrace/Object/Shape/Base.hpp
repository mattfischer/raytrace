#ifndef OBJECT_SHAPE_BASE_HPP
#define OBJECT_SHAPE_BASE_HPP

#include "Math/Ray.hpp"
#include "Math/Normal.hpp"
#include "Math/Point2D.hpp"
#include "Math/Transformation.hpp"

#include "Object/BoundingVolume.hpp"

#include <vector>
#include <memory>

namespace Object {
	namespace Shape {
		class Base
		{
		public:
			struct Intersection {
				float distance;
				Math::Normal normal;
				Math::Vector du;
				Math::Vector dv;
				Math::Point2D surfacePoint;
			};

			class Sampler {
			public:
				virtual float surfaceArea() const = 0;
				virtual bool sample(const Math::Point2D &surfacePoint, Math::Point &point, Math::Normal &normal) const = 0;
			};

			virtual bool intersect(const Math::Ray &ray, Intersection &intersection) const = 0;
			virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const = 0;
			virtual const Sampler *sampler() const { return nullptr; }
		};
	}
}

#endif