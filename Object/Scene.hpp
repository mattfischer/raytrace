#ifndef OBJECT_SCENE_HPP
#define OBJECT_SCENE_HPP

#include "Object/Camera.hpp"
#include "Object/Primitive.hpp"
#include "Object/PointLight.hpp"
#include "Object/BoundingVolumeHierarchy.hpp"
#include "Object/Intersection.hpp"

#include "Object/CLProxies.hpp"
#include "OpenCL.hpp"

#include <vector>
#include <memory>

namespace Object {
    class Scene
    {
    public:
        Scene(std::unique_ptr<Camera> &&camera, std::vector<std::unique_ptr<Primitive>> &&primitives, std::vector<std::unique_ptr<PointLight>> &&pointLights, const Math::Radiance &skyRadiance);

        const Camera &camera() const;
        const std::vector<std::unique_ptr<Primitive>> &primitives() const;
        const std::vector<std::reference_wrapper<Primitive>> &areaLights() const;
        const std::vector<std::unique_ptr<PointLight>> &pointLights() const;
        const Math::Radiance &skyRadiance() const;

        const Object::BoundingVolumeHierarchy &boundingVolumeHierarchy() const;

        Object::Intersection intersect(const Math::Beam &beam, float maxDistance, bool closest) const;

        void writeProxy(SceneProxy &proxy, OpenCL::Allocator &clAllocator) const;

    protected:
        std::unique_ptr<Camera> mCamera;
        std::vector<std::unique_ptr<Primitive>> mPrimitives;
        std::vector<std::reference_wrapper<Primitive>> mAreaLights;
        std::vector<std::unique_ptr<PointLight>> mPointLights;
        Math::Radiance mSkyRadiance;

        Object::BoundingVolumeHierarchy mBoundingVolumeHierarchy;
    };
}

#endif
