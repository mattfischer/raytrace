#include "Object/Shape/BezierPatches.hpp"

namespace Object {
    namespace Shape {
        BezierPatches::BezierPatches(std::vector<std::unique_ptr<BezierPatch>> &&patches)
            : mPatches(std::move(patches))
        {
            Math::Transformation transformation;

            for (const std::unique_ptr<BezierPatch> &patch : mPatches) {
                mVolumes.push_back(patch->boundingVolume(transformation));
            }
        }

        bool BezierPatches::intersect(const Math::Ray &ray, Intersection &isect) const
        {
            bool ret = false;

            BoundingVolume::RayData rayData = BoundingVolume::getRayData(ray);
            for(int i=0; i<mPatches.size(); i++) {
                float distance;
                if (mVolumes[i].intersectRay(rayData, distance) && distance < isect.distance) {
                    if (mPatches[i]->intersect(ray, isect)) {
                        ret = true;
                    }
                }
            }

            return ret;
        }

        BoundingVolume BezierPatches::boundingVolume(const Math::Transformation &trans) const
        {
            BoundingVolume volume;

            for (const std::unique_ptr<BezierPatch> &patch : mPatches) {
                volume.expand(patch->boundingVolume(trans));
            }

            return volume;
        }

        void BezierPatches::writeProxy(ShapeProxy &proxy, OpenCL::Allocator &clAllocator) const
        {
            proxy.type = ShapeProxy::Type::Grids;
            proxy.grids.numChildren = mPatches.size();
            proxy.grids.grids = clAllocator.allocateArray<GridProxy>(proxy.grids.numChildren);
            proxy.grids.volumes = clAllocator.allocateArray<BoundingVolumeProxy>(proxy.grids.numChildren);
            for(int i=0; i<mPatches.size(); i++) {
                mPatches[i]->writeProxy(proxy.grids.grids[i], clAllocator);
                mVolumes[i].writeProxy(proxy.grids.volumes[i]);
            }
        }
    }
}
