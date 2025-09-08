#ifndef OBJECT_SCENE_HPP
#define OBJECT_SCENE_HPP

#include "Object/Camera.hpp"
#include "Object/Primitive.hpp"
#include "Object/BoundingVolumeHierarchy.hpp"
#include "Object/Intersection.hpp"
#include "Object/Light.hpp"
#include "Object/Impl/Light/Point.hpp"

#include "Object/CLProxies.hpp"
#include "OpenCL.hpp"

#include <vector>
#include <memory>

namespace Object {
    class Scene
    {
    public:
        Scene(std::unique_ptr<Camera> camera, std::vector<std::unique_ptr<Primitive>> primitives, std::vector<std::unique_ptr<Object::Light>> lights, const Math::Radiance &skyRadiance);

        const Camera &camera() const;
        const std::vector<std::unique_ptr<Primitive>> &primitives() const;
        const std::vector<std::reference_wrapper<Object::Primitive>> &areaLights() const;
        const std::vector<std::reference_wrapper<Object::Light>> &lights() const;

        const Math::Radiance &skyRadiance() const;

        const Object::BoundingVolumeHierarchy &boundingVolumeHierarchy() const;

        Object::Intersection intersect(const Math::Beam &beam, float maxDistance, bool closest) const;

        void writeProxy(SceneProxy &proxy, OpenCL::Allocator &clAllocator) const;

    protected:
        std::unique_ptr<Camera> mCamera;
        std::vector<std::unique_ptr<Object::Primitive>> mPrimitives;
        std::vector<std::unique_ptr<Object::Light>> mExplicitLights;
        std::vector<std::reference_wrapper<Object::Light>> mLights;
        std::vector<std::reference_wrapper<Object::Primitive>> mAreaLights;
        std::vector<std::reference_wrapper<Object::Impl::Light::Point>> mPointLights;

        Math::Radiance mSkyRadiance;

        Object::BoundingVolumeHierarchy mBoundingVolumeHierarchy;
    };
}

#endif
