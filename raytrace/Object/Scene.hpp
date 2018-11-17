#ifndef OBJECT_SCENE_HPP
#define OBJECT_SCENE_HPP

#include "Object/Camera.hpp"
#include "Object/Primitive.hpp"
#include "Object/Light.hpp"
#include "Object/BoundingVolumeHierarchy.hpp"

#include <vector>
#include <memory>

namespace Object {
	class Scene
	{
	public:
		Scene(std::unique_ptr<Camera> &&camera, std::vector<std::unique_ptr<Primitive>> &&primitives, std::vector<std::unique_ptr<Light>> &&lights);

		const Camera &camera() const;
		const std::vector<std::unique_ptr<Primitive>> &primitives() const;
		const std::vector<std::reference_wrapper<Primitive>> &areaLights() const;
		const std::vector<std::unique_ptr<Light>> &lights() const;

		const Object::BoundingVolumeHierarchy &boundingVolumeHierarchy() const;

		bool intersect(const Math::Ray &ray, Shape::Base::Intersection &shapeIntersection, Object::Primitive *&primitive) const;

	protected:
		std::unique_ptr<Camera> mCamera;
		std::vector<std::unique_ptr<Primitive>> mPrimitives;
		std::vector<std::reference_wrapper<Primitive>> mAreaLights;
		std::vector<std::unique_ptr<Light>> mLights;

		Object::BoundingVolumeHierarchy mBoundingVolumeHierarchy;
	};
}

#endif
