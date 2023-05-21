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

        bool Group::intersect(const Math::Ray &ray, Intersection &isect) const
        {
            bool ret = false;

            BoundingVolume::RayData rayData = BoundingVolume::getRayData(ray);
            for(int i=0; i<mShapes.size(); i++) {
                float distance;
                if (mVolumes[i].intersectRay(rayData, distance) && distance < isect.distance) {
                    if (mShapes[i]->intersect(ray, isect)) {
                        ret = true;
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
    }
}
