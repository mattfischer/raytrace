#ifndef OBJECT_SHAPE_MODEL_HPP
#define OBJECT_SHAPE_MODEL_HPP

#include "Object/Shape/Base.hpp"
#include "Object/Shape/Group.hpp"

namespace Object {
	namespace Shape {
		class Model : public Base
		{
		public:
			Model(const std::string &filename);

			virtual bool intersect(const Math::Ray &ray, Intersection &intersection) const;
			virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const;

		private:
			std::unique_ptr<Group> mGroup;
		};
	}
}

#endif