#ifndef OBJECT_SHAPE_SPHERE_HPP
#define OBJECT_SHAPE_SPHERE_HPP

#include "Object/Shape/Base.hpp"

namespace Object {
	namespace Shape {
		class Sphere : public Base
		{
		public:
			Sphere(const Math::Point &position, float radius);

			virtual bool intersect(const Math::Ray &ray, Intersection &intersection) const;
			virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const;
			virtual bool sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const;

		private:
			Math::Point mPosition;
			float mRadius;
		};
	}
}

#endif
