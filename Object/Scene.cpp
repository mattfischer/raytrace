#include "Object/Scene.hpp"

#include "Object/Camera.hpp"
#include "Object/Primitive.hpp"
#include "Object/PointLight.hpp"

#include "Object/Shape/Transformed.hpp"
#include "Object/Albedo/Solid.hpp"
#include "Object/Brdf/Lambert.hpp"
#include "Object/Brdf/Phong.hpp"

#include <cfloat>

namespace Object {
    Scene::Scene(std::unique_ptr<Camera> &&camera, std::vector<std::unique_ptr<Primitive>> &&primitives, std::vector<std::unique_ptr<PointLight>> &&pointLights, const Math::Radiance &skyRadiance)
        : mCamera(std::move(camera))
        , mPrimitives(std::move(primitives))
        , mPointLights(std::move(pointLights))
        , mSkyRadiance(skyRadiance)
    {
        std::vector<Math::Point> centroids;
        centroids.reserve(mPrimitives.size());

        for (std::unique_ptr<Primitive> &primitive : mPrimitives) {
            centroids.push_back(primitive->boundingVolume().centroid());

            if (primitive->surface().radiance().magnitude() > 0) {
                mAreaLights.push_back(static_cast<Object::Primitive&>(*primitive));
            }
        }

        auto func = [&](int index) {
            return mPrimitives[index]->boundingVolume();
        };

        mBoundingVolumeHierarchy = Object::BoundingVolumeHierarchy(std::move(centroids), func);
    }

    const Camera &Scene::camera() const
    {
        return *mCamera;
    }

    const std::vector<std::unique_ptr<Primitive>> &Scene::primitives() const
    {
        return mPrimitives;
    }

    const std::vector<std::reference_wrapper<Primitive>> &Scene::areaLights() const
    {
        return mAreaLights;
    }

    const std::vector<std::unique_ptr<PointLight>> &Scene::pointLights() const
    {
        return mPointLights;
    }

    const Math::Radiance &Scene::skyRadiance() const
    {
        return mSkyRadiance;
    }

    const Object::BoundingVolumeHierarchy &Scene::boundingVolumeHierarchy() const
    {
        return mBoundingVolumeHierarchy;
    }

    Object::Intersection Scene::intersect(const Math::Beam &beam, float maxDistance, bool closest) const
    {
        Object::BoundingVolume::RayData rayData = Object::BoundingVolume::getRayData(beam.ray());

        Object::Shape::Base::Intersection shapeIntersection;
        shapeIntersection.distance = maxDistance;
        Object::Primitive *primitive = 0;

        auto func = [&](int index, float &maxDistance) {
            if (mPrimitives[index]->shape().intersect(beam.ray(), shapeIntersection, closest)) {
                primitive = mPrimitives[index].get();
                maxDistance = shapeIntersection.distance;
                return true;
            }

            return false;
        };

        mBoundingVolumeHierarchy.intersect(rayData, shapeIntersection.distance, closest, std::ref(func));

        if (primitive) {
            return Object::Intersection(*this, *primitive, beam, shapeIntersection);
        }
        else {
            return Object::Intersection();
        }
    }

    void Scene::writeProxy(SceneProxy &proxy, OpenCL::Allocator &clAllocator) const
    {
        proxy.numPrimitives = mPrimitives.size();
        proxy.primitives = clAllocator.allocateArray<PrimitiveProxy>(proxy.numPrimitives);
        proxy.numAreaLights = mAreaLights.size();
        proxy.areaLights = clAllocator.allocateArray<PrimitiveProxy*>(proxy.numAreaLights);

        int n = 0;
        for(int i=0; i<mPrimitives.size(); i++) {
            mPrimitives[i]->writeProxy(proxy.primitives[i], clAllocator);
            if(mPrimitives[i]->surface().radiance().magnitude() > 0) {
                proxy.areaLights[n++] = &proxy.primitives[i];
            }
        }

        proxy.numPointLights = mPointLights.size();
        proxy.pointLights = clAllocator.allocateArray<PointLightProxy>(proxy.numPointLights);
        for(int i=0; i<mPointLights.size(); i++) {
            mPointLights[i]->writeProxy(proxy.pointLights[i]);
        }

        mSkyRadiance.writeProxy(proxy.skyRadiance);
        mCamera->writeProxy(proxy.camera);
        proxy.bvh = clAllocator.allocateArray<BVHNodeProxy>(mBoundingVolumeHierarchy.nodes().size());
        mBoundingVolumeHierarchy.writeProxy(proxy.bvh);
    }
}
