#define _USE_MATH_DEFINES
#include "Render/Cpu/Lighter/IrradianceCached.hpp"
#include "Render/Cpu/RasterJob.hpp"

#include "Object/Scene.hpp"

#include "Math/OrthonormalBasis.hpp"
#include "Math/Sampler/Random.hpp"

#include <cmath>
#include <cfloat>
#include <functional>
#include <mutex>

namespace Render {
        namespace Cpu {
        namespace Lighter
        {
            class RadianceGradient
            {
            public:
                RadianceGradient() = default;
                RadianceGradient(const Math::Radiance &radiance, const Math::Vector &vector)
                {
                    mRed = vector * radiance.red();
                    mGreen = vector * radiance.green();
                    mBlue = vector * radiance.blue();
                }

                RadianceGradient(const Math::Vector &red, const Math::Vector &green, const Math::Vector &blue)
                {
                    mRed = red;
                    mGreen = green;
                    mBlue = blue;
                }

                Math::Radiance operator*(const Math::Vector &vector) const
                {
                    return Math::Radiance(mRed * vector, mGreen * vector, mBlue * vector);
                }

                RadianceGradient operator+(const RadianceGradient &other) const
                {
                    return RadianceGradient(mRed + other.mRed, mGreen + other.mGreen, mBlue + other.mBlue);
                }
            
                RadianceGradient& operator+=(const RadianceGradient &other)
                {
                    mRed = mRed + other.mRed;
                    mGreen = mGreen + other.mGreen;
                    mBlue = mBlue + other.mBlue;

                    return *this;
                }

                RadianceGradient operator*(float other) const
                {
                    return RadianceGradient(mRed * other, mGreen * other, mBlue * other);
                }

                RadianceGradient operator/(float other) const
                {
                    return RadianceGradient(mRed / other, mGreen / other, mBlue / other);
                }

            private:
                Math::Vector mRed;
                Math::Vector mGreen;
                Math::Vector mBlue;
            };

            class IrradianceCached::Cache
            {
            public:
                struct Entry {
                    Math::Point point;
                    Math::Normal normal;
                    float radius;
                    Math::Radiance radiance;
                    RadianceGradient rotGrad;
                    RadianceGradient transGrad;
                };

                Cache(float threshold);
                float threshold() const;

                float weight(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const;
                float error(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const;
                bool test(const Math::Point &point, const Math::Normal &normal) const;
                bool testUnlocked(const Math::Point &point, const Math::Normal &normal) const;
                Math::Radiance interpolate(const Math::Point &point, const Math::Normal &normal) const;
                Math::Radiance interpolateUnlocked(const Math::Point &point, const Math::Normal &normal) const;
                void add(const Entry &entry);
                void clear();

            private:
                struct OctreeNode
                {
                    std::vector<Entry> entries;
                    std::unique_ptr<OctreeNode> children[8];
                };

                bool visitOctreeNode(OctreeNode *node, const Math::Point &origin, float size, const Math::Point &point, const std::function<bool(const Entry &)> &callback) const;

                float distance2ToNode(const Math::Point &point, int idx, const Math::Point &origin, float size) const;
                void getChildNode(const Math::Point &origin, float size, int idx, Math::Point &childOrigin, float &childSize) const;
                bool isEntryValid(const Entry &entry, const Math::Point &point, const Math::Normal &normal, float weight, float threshold) const;

                std::unique_ptr<OctreeNode> mOctreeRoot;
                Math::Point mOctreeOrigin;
                float mOctreeSize;
                mutable std::mutex mMutex;
                float mThreshold;
            };

            IrradianceCached::Cache::Cache(float threshold)
            {
                mOctreeSize = 0;
                mThreshold = threshold;
            }

            float IrradianceCached::Cache::threshold() const
            {
                return mThreshold;
            }

            float IrradianceCached::Cache::weight(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const 
            {
                //return std::pow(std::max(double(0), 1.0f - (point - entry.point).magnitude2() / (1.0 * entry.radius * std::pow(normal * entry.normal, 4.0f))), 2);
                return 1.0f / ((point - entry.point).magnitude() / entry.radius + std::sqrt(1 - std::min(1.0f, normal * entry.normal)));
            }

            float IrradianceCached::Cache::error(const Entry &entry, const Math::Point &point, const Math::Normal &normal) const
            {
                return ((4.0f / M_PI) * (point - entry.point).magnitude() / entry.radius + std::sqrt(1 - normal * entry.normal));
            }

            float IrradianceCached::Cache::distance2ToNode(const Math::Point &point, int idx, const Math::Point &origin, float size) const
            {
                float distance2 = 0;
                float d;

                if (point.x() < origin.x() - size) { d = ((origin.x() - size) - point.x()); distance2 += d * d; }
                if (point.y() < origin.y() - size) { d = ((origin.y() - size) - point.y()); distance2 += d * d; }
                if (point.z() < origin.z() - size) { d = ((origin.z() - size) - point.z()); distance2 += d * d; }
                if (point.x() > origin.x() + size) { d = (point.x() - (origin.x() + size)); distance2 += d * d; }
                if (point.y() > origin.y() + size) { d = (point.y() - (origin.y() + size)); distance2 += d * d; }
                if (point.z() > origin.z() + size) { d = (point.z() - (origin.z() + size)); distance2 += d * d; }

                return distance2;
            }

            void IrradianceCached::Cache::getChildNode(const Math::Point &origin, float size, int idx, Math::Point &childOrigin, float &childSize) const
            {
                float x = (idx & 1) ? 1.0f : -1.0f;
                float y = (idx & 2) ? 1.0f : -1.0f;
                float z = (idx & 4) ? 1.0f : -1.0f;

                childSize = size / 2;
                childOrigin = origin + Math::Vector(x, y, z) * childSize;
            }

            bool IrradianceCached::Cache::isEntryValid(const Entry &entry, const Math::Point &point, const Math::Normal &normal, float weight, float threshold) const
            {
                float d = (point - entry.point) * ((normal + entry.normal) / 2);
                return (d >= -0.01 && weight > 1 / threshold);
            }

            bool IrradianceCached::Cache::visitOctreeNode(OctreeNode *node, const Math::Point &origin, float size, const Math::Point &point, const std::function<bool(const Entry &)> &callback) const
            {
                if (!node) {
                    return true;
                }

                for (const Entry &entry : node->entries)
                {
                    if (!callback(entry)) {
                        return false;
                    }
                }

                for (int i = 0; i < 8; i++) {
                    Math::Point childOrigin;
                    float childSize;

                    getChildNode(origin, size, i, childOrigin, childSize);

                    float distance2 = distance2ToNode(point, i, childOrigin, childSize);
                    if (distance2 < childSize * childSize) {
                        if (!visitOctreeNode(node->children[i].get(), childOrigin, childSize, point, callback)) {
                            return false;
                        }
                    }
                }

                return true;
            }

            bool IrradianceCached::Cache::test(const Math::Point &point, const Math::Normal &normal) const
            {
                std::lock_guard<std::mutex> guard(mMutex);
                return testUnlocked(point, normal);
            }

            bool IrradianceCached::Cache::testUnlocked(const Math::Point &point, const Math::Normal &normal) const
            {
                bool ret = false;
                auto callback = [&](const Entry &entry) {
                    float w = weight(entry, point, normal);
                    if (isEntryValid(entry, point, normal, w, mThreshold)) {
                        ret = true;
                        return false;
                    }
                    return true;
                };

                visitOctreeNode(mOctreeRoot.get(), mOctreeOrigin, mOctreeSize, point, std::ref(callback));

                return ret;
            }

            Math::Radiance IrradianceCached::Cache::interpolate(const Math::Point &point, const Math::Normal &normal) const
            {
                std::lock_guard<std::mutex> guard(mMutex);
                return interpolateUnlocked(point, normal);
            }

            Math::Radiance IrradianceCached::Cache::interpolateUnlocked(const Math::Point &point, const Math::Normal &normal) const
            {
                float totalWeight = 0;
                Math::Radiance irradiance;
                float threshold = mThreshold;

                auto callback = [&] (const Entry &entry) {
                    float w = weight(entry, point, normal);
                    if (isEntryValid(entry, point, normal, w, threshold)) {
                        if (std::isinf(w)) {
                            irradiance = entry.radiance;
                            totalWeight = 1;
                            return false;
                        }
                        Math::Vector cross = Math::Vector(normal % entry.normal);
                        Math::Vector dist = point - entry.point;
                        irradiance += (entry.radiance + entry.rotGrad * cross + entry.transGrad * dist) * w;
                        totalWeight += w;
                    }
                    return true;
                };

                for(int i=0; i<3; i++) {
                    visitOctreeNode(mOctreeRoot.get(), mOctreeOrigin, mOctreeSize, point, std::ref(callback));

                    if (totalWeight > 0) {
                        irradiance = irradiance / totalWeight;
                        break;
                    }
                    else {
                        threshold *= 2;
                    }
                }

                return irradiance.clamp();
            }

            void IrradianceCached::Cache::add(const Entry &entry)
            {
                std::lock_guard<std::mutex> guard(mMutex);

                float R = entry.radius * mThreshold;

                if (!mOctreeRoot) {
                    mOctreeRoot = std::make_unique<OctreeNode>();
                    mOctreeSize = R;
                    mOctreeOrigin = entry.point;
                }

                while (std::abs(entry.point.x() - mOctreeOrigin.x()) > mOctreeSize ||
                    std::abs(entry.point.y() - mOctreeOrigin.y()) > mOctreeSize ||
                    std::abs(entry.point.z() - mOctreeOrigin.z()) > mOctreeSize ||
                    mOctreeSize < R) {
                    float x = (entry.point.x() > mOctreeOrigin.x()) ? 1.0f : -1.0f;
                    float y = (entry.point.y() > mOctreeOrigin.y()) ? 1.0f : -1.0f;
                    float z = (entry.point.z() > mOctreeOrigin.z()) ? 1.0f : -1.0f;

                    int idx = ((x < 0) ? 1 : 0) + ((y < 0) ? 2 : 0) + ((z < 0) ? 4 : 0);
                    std::unique_ptr<OctreeNode> newRoot = std::make_unique<OctreeNode>();
                    newRoot->children[idx] = std::move(mOctreeRoot);
                    mOctreeRoot = std::move(newRoot);
                    mOctreeOrigin = mOctreeOrigin + Math::Vector(x, y, z) * mOctreeSize;
                    mOctreeSize *= 2;
                }

                Math::Point origin = mOctreeOrigin;
                float size = mOctreeSize;
                OctreeNode *node = mOctreeRoot.get();
                while (size > R * 2) {
                    float x = (entry.point.x() > origin.x()) ? 1.0f : -1.0f;
                    float y = (entry.point.y() > origin.y()) ? 1.0f : -1.0f;
                    float z = (entry.point.z() > origin.z()) ? 1.0f : -1.0f;

                    int idx = ((x > 0) ? 1 : 0) + ((y > 0) ? 2 : 0) + ((z > 0) ? 4 : 0);
                    Math::Point newOrigin = origin + Math::Vector(x, y, z) * size / 2;

                    if (!node->children[idx])
                    {
                        node->children[idx] = std::make_unique<OctreeNode>();
                    }

                    origin = newOrigin;
                    size /= 2;
                    node = node->children[idx].get();
                }

                node->entries.push_back(entry);
            }

            void IrradianceCached::Cache::clear()
            {
                mOctreeRoot.release();
            }

            IrradianceCached::IrradianceCached(const Settings &settings)
                : mSettings(settings)
            {
                mDirectLighter = std::make_unique<Lighter::Direct>();
                mUniPathLighter = std::make_unique<Lighter::UniPath>();
                mCache = std::make_unique<Cache>(settings.cacheThreshold);
            }

            Math::Radiance IrradianceCached::light(const Object::Intersection &isect, Math::Sampler::Base &sampler) const
            {
                const Object::Surface &surface = isect.primitive().surface();
                const Math::Point &pnt = isect.point();
                const Math::Normal &nrmFacing = isect.facingNormal();
                const Math::Color &albedo = isect.albedo();

                Math::Radiance rad = mDirectLighter->light(isect, sampler);

                if(surface.lambert() > 0) {
                    Math::Radiance irad = mCache->interpolateUnlocked(pnt, nrmFacing);
                    rad += irad * albedo * surface.lambert() / (float)M_PI;
                }

                return rad;
            }

            struct ThreadLocal : public Executor::Job::ThreadLocal {
                Math::Sampler::Random sampler;
            };

            std::vector<std::unique_ptr<Render::Cpu::Executor::Job>> IrradianceCached::createPrerenderJobs(const Object::Scene &scene, Render::Framebuffer &framebuffer)
            {
                std::unique_ptr<Executor::Job> job = std::make_unique<Render::Cpu::RasterJob>(
                    framebuffer.width(),
                    framebuffer.height(),
                    1,
                    [&]() { return std::make_unique<ThreadLocal>(); },
                    [&](int x, int y, int sample, Executor::Job::ThreadLocal &threadLocalBase)
                        {
                            prerenderPixel(x, y, framebuffer, scene, static_cast<ThreadLocal&>(threadLocalBase).sampler);
                        }
                );

                std::vector<std::unique_ptr<Render::Cpu::Executor::Job>> jobs;
                jobs.push_back(std::move(job));
                return jobs;
            }

            void IrradianceCached::prerenderPixel(unsigned int x, unsigned int y, Render::Framebuffer &framebuffer, const Object::Scene &scene, Math::Sampler::Base &sampler)
            {
                Math::Color pixelColor;
                sampler.startSample(x, y, 0);
                Math::Beam beam = scene.camera().createPixelBeam(Math::Point2D(float(x), float(y)), framebuffer.width(), framebuffer.height(), Math::Point2D());

                Object::Intersection isect = scene.intersect(beam);

                const Object::Surface &surface = isect.primitive().surface();

                if (isect.valid() && surface.lambert() > 0) {
                    const Math::Point &pnt = isect.point();
                    const Math::Normal &nrmFacing = isect.facingNormal();

                    if (!mCache->test(pnt, nrmFacing)) {
                        Math::OrthonormalBasis basis(nrmFacing);

                        float mean = 0;
                        int den = 0;
                        Math::Radiance rad;
                        const unsigned int M = static_cast<unsigned int>(std::sqrt(mSettings.indirectSamples));
                        const unsigned int N = static_cast<unsigned int>(mSettings.indirectSamples / M);
                        std::vector<Math::Radiance> samples;
                        std::vector<float> sampleDistances;
                        samples.resize(M * N);
                        sampleDistances.resize(M * N);
                        for (unsigned int k = 0; k < N; k++) {
                            for (unsigned int j = 0; j < M; j++) {
                                sampler.startSample();

                                float phi = 2 * M_PI * (k + sampler.getValue()) / N;
                                float theta = std::asin(std::sqrt((j + sampler.getValue()) / M));
                                Math::Vector dirIn = basis.localToWorld(Math::Vector::fromPolar(phi, theta, 1));

                                Math::Point pntOffset = pnt + Math::Vector(nrmFacing) * 0.01f;
                                Math::Ray ray(pntOffset, dirIn);
                                Math::Beam beam(ray, Math::Bivector(), Math::Bivector());
                                Object::Intersection isect2 = scene.intersect(beam);

                                if (isect2.valid()) {
                                    mean += 1 / isect2.distance();
                                    den++;
                                    Math::Radiance rad2 = mUniPathLighter->light(isect2, sampler);
                                    rad2 = rad2 - isect2.primitive().surface().radiance();

                                    samples[k * M + j] = rad2;
                                    sampleDistances[k * M + j] = isect2.distance();

                                    rad += rad2 * (float)M_PI / (float)(M * N);
                                }
                                else {
                                    sampleDistances[k * M + j] = static_cast<float>(FLT_MAX);
                                }
                            }
                        }

                        if (mean > 0) {
                            mean = den / mean;

                            Cache::Entry newEntry;
                            newEntry.point = pnt;
                            newEntry.normal = nrmFacing;
                            newEntry.radiance = rad;

                            float radius = mean;
                            float projectedPixelSize = scene.camera().projectSize(2.0f / framebuffer.width(), isect.distance());
                            float minRadius = 3 * projectedPixelSize / mCache->threshold();
                            float maxRadius = 20 * minRadius;
                            newEntry.radius = std::min(std::max(radius, minRadius), maxRadius);

                            RadianceGradient transGrad;
                            RadianceGradient rotGrad;
                            for (unsigned int k = 0; k < N; k++) {
                                unsigned int k1 = (k > 0) ? (k - 1) : N - 1;
                                float phi = 2 * M_PI * k / N;
                                Math::Vector u = basis.localToWorld(Math::Vector::fromPolar(phi, 0, 1));
                                Math::Vector v = basis.localToWorld(Math::Vector::fromPolar(phi + M_PI / 2, 0, 1));

                                for (unsigned int j = 0; j < M; j++) {
                                    float thetaMinus = std::asin(std::sqrt(float(j) / M));
                                    float thetaPlus = std::asin(std::sqrt(float(j + 1) / M));

                                    if (j > 0) {
                                        unsigned int j1 = j - 1;

                                        Math::Vector c = u * std::sin(thetaMinus) * std::cos(thetaMinus) * std::cos(thetaMinus) * 2 * (float)M_PI / (N * std::min(sampleDistances[k * M + j], sampleDistances[k * M + j1]));
                                        transGrad += RadianceGradient(samples[k * M + j] - samples[k * M + j1], c);
                                    }

                                    Math::Vector c = v * (std::sin(thetaPlus) - std::sin(thetaMinus)) / std::min(sampleDistances[k * M + j], sampleDistances[k1 * M + j]);
                                    transGrad += RadianceGradient(samples[k * M + j] - samples[k1 * M + j], c);

                                    rotGrad += RadianceGradient(samples[k * M + j], v) * std::tan(thetaMinus) * (float)M_PI / (float)(M * N);
                                }
                            }

                            if (radius < minRadius) {
                                transGrad = transGrad * radius / minRadius;
                            }

                            newEntry.transGrad = transGrad;
                            newEntry.rotGrad = rotGrad;

                            mCache->add(newEntry);
                            pixelColor = Math::Color(1, 1, 1);
                        }
                    }
                }

                framebuffer.setPixel(x, y, pixelColor);
            }
        }
    }
}