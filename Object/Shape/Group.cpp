#include "Object/Shape/Group.hpp"

namespace Object {
    namespace Shape {
        Group::Group(std::vector<std::unique_ptr<Base>> &&shapes)
            : mShapes(std::move(shapes))
        {
            Math::Transformation transformation;

            for (const std::unique_ptr<Base> &shape : mShapes) {
                mVolumes.push_back(shape->boundingVolume(transformation));
            }
        }

        bool Group::intersect(const Math::Ray &ray, Intersection &isect, bool closest) const
        {
            bool ret = false;

            BoundingVolume::RayData rayData = BoundingVolume::getRayData(ray);
            for(int i=0; i<mShapes.size(); i++) {
                float distance;
                if (mVolumes[i].intersectRay(rayData, distance) && distance < isect.distance) {
                    if (mShapes[i]->intersect(ray, isect, closest)) {
                        ret = true;
                        if(!closest) {
                            break;
                        }
                    }
                }
            }

            return ret;
        }

        BoundingVolume Group::boundingVolume(const Math::Transformation &trans) const
        {
            BoundingVolume volume;

            for (const std::unique_ptr<Base> &shape : mShapes) {
                volume.expand(shape->boundingVolume(trans));
            }

            return volume;
        }

        void Group::writeProxy(ShapeProxy &proxy, OpenCL::Allocator &clAllocator) const
        {
            proxy.type = ShapeProxy::Type::Group;
            proxy.group.numShapes = mShapes.size();
            proxy.group.shapes = clAllocator.allocateArray<ShapeProxy>(proxy.group.numShapes);
            proxy.group.volumes = clAllocator.allocateArray<BoundingVolumeProxy>(proxy.group.numShapes);
            for(int i=0; i<mShapes.size(); i++) {
                mShapes[i]->writeProxy(proxy.group.shapes[i], clAllocator);
                mVolumes[i].writeProxy(proxy.group.volumes[i]);
            }
        }
    }
}
