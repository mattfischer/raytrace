#ifndef OBJECT_SHAPE_BASE_HPP
#define OBJECT_SHAPE_BASE_HPP

#include "Math/Ray.hpp"
#include "Math/Normal.hpp"
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
			};

			class Sampler {
			public:
				virtual float surfaceArea() const = 0;
				virtual bool sample(float u, float v, Math::Point &point, Math::Normal &normal) const = 0;
			};

			virtual bool intersect(const Math::Ray &ray, Intersection &intersection) const = 0;
			virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const = 0;
			virtual const Sampler *sampler() const { return nullptr; }
		};
	}
}

#endif