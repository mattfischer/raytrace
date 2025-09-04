#include "Object/Impl/Shape/BezierPatch.hpp"

namespace Object::Impl::Shape {
    BezierPatch::BezierPatch(int width, int height, std::vector<Math::Point> &&controlPoints)
    {
        std::vector<Grid::Vertex> vertices;

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                float s = float(i) / (width - 1);
                float t = float(j) / (height - 1);
                float s1 = float((i < width / 2) ? (i + 1) : (i - 1)) / (width - 1);
                float t1 = float((j < height / 2) ? (j + 1) : (j - 1)) / (height - 1);

                float Bs[4] = { (1 - s)*(1 - s)*(1 - s), 3 * s*(1 - s)*(1 - s), 3 * s*s*(1 - s), s*s*s };
                float Bt[4] = { (1 - t)*(1 - t)*(1 - t), 3 * t*(1 - t)*(1 - t), 3 * t*t*(1 - t), t*t*t };
                float Bds[4] = { -3 * (1 - s)*(1 - s), 3 * (1 - s)*(1 - s) - 3 * 2 * s * (1 - s), 3 * 2 * s*(1 - s) - 3 * s * s, 3 * s*s };
                float Bdt[4] = { -3 * (1 - t)*(1 - t), 3 * (1 - t)*(1 - t) - 3 * 2 * t * (1 - t), 3 * 2 * t*(1 - t) - 3 * t * t, 3 * t*t };

                float Bs1[4] = { (1 - s1)*(1 - s1)*(1 - s1), 3 * s1*(1 - s1)*(1 - s1), 3 * s1*s1*(1 - s1), s1*s1*s1 };
                float Bt1[4] = { (1 - t1)*(1 - t1)*(1 - t1), 3 * t1*(1 - t1)*(1 - t1), 3 * t1*t1*(1 - t1), t1*t1*t1 };
                float Bds1[4] = { -3 * (1 - s1)*(1 - s1), 3 * (1 - s1)*(1 - s1) - 3 * 2 * s1 * (1 - s1), 3 * 2 * s1*(1 - s1) - 3 * s1 * s1, 3 * s1*s1 };
                float Bdt1[4] = { -3 * (1 - t1)*(1 - t1), 3 * (1 - t1)*(1 - t1) - 3 * 2 * t1 * (1 - t1), 3 * 2 * t1*(1 - t1) - 3 * t1 * t1, 3 * t1*t1 };

                Math::Vector p;
                Math::Vector ds;
                Math::Vector dt;
                Math::Vector ds1;
                Math::Vector dt1;
                for (int k = 0; k < 4; k++) {
                    for (int l = 0; l < 4; l++) {
                        p = p + Math::Vector(controlPoints[k * 4 + l]) * (Bs[l] * Bt[k]);
                        ds = ds + Math::Vector(controlPoints[k * 4 + l]) * (Bds[l] * Bt[k]);
                        dt = dt + Math::Vector(controlPoints[k * 4 + l]) * (Bs[l] * Bdt[k]);
                        ds1 = ds1 + Math::Vector(controlPoints[k * 4 + l]) * (Bds1[l] * Bt1[k]);
                        dt1 = dt1 + Math::Vector(controlPoints[k * 4 + l]) * (Bs1[l] * Bdt1[k]);
                    }
                }

                if (ds.magnitude2() < 0.0000001) {
                    ds = ds1;
                }
                if (dt.magnitude2() < 0.0000001) {
                    dt = dt1;
                }

                Grid::Vertex vertex;
                vertex.point = Math::Point(p);
                vertex.tangent = Math::Bivector(ds, dt);
                vertex.normal = Math::Normal(ds % dt).normalize();

                vertices.push_back(vertex);
            }
        }

        mGrid = std::make_unique<Grid>(width, height, std::move(vertices));
    }

    bool BezierPatch::intersect(const Math::Ray &ray, Intersection &isect, bool closest) const
    {
        return mGrid->intersect(ray, isect, closest);
    }

    BoundingVolume BezierPatch::boundingVolume(const Math::Transformation &trans) const
    {
        return mGrid->boundingVolume(trans);
    }

    void BezierPatch::writeProxy(ShapeProxy &proxy, OpenCL::Allocator &clAllocator) const
    {
        return mGrid->writeProxy(proxy, clAllocator);
    }
}