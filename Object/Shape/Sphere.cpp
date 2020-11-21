#include "Object/Shape/Sphere.hpp"
#include "Math/Transformation.hpp"

#include <math.h>

namespace Object {
    namespace Shape {
        Sphere::Sphere(const Math::Point &position, float radius)
            : mPosition(position), mRadius(radius)
        {
        }

        bool Sphere::intersect(const Math::Ray &ray, Intersection &intersection) const
        {
            float a, b, c;
            float disc;

            a = ray.direction().magnitude2();
            b = 2 * ((ray.origin() - mPosition) * ray.direction());
            c = (ray.origin() - mPosition).magnitude2() - mRadius * mRadius;

            disc = b * b - 4 * a * c;
            if(disc >= 0)
            {
                float distance = (-b - sqrt(disc)) / (2 * a);

                if(distance >= 0 && distance < intersection.distance)
                {
                    intersection.distance = distance;
                    Math::Point point = ray.origin() + ray.direction() * distance;
                    intersection.normal = Math::Normal(point - mPosition) / mRadius;
                    intersection.surfacePoint = Math::Point2D();
                    intersection.tangent = Math::Bivector(Math::Vector(), Math::Vector());
                    return true;
                }

                distance = (-b + sqrt(disc)) / (2 * a);

                if(distance >= 0 && distance < intersection.distance)
                {
                    intersection.distance = distance;
                    Math::Point point = ray.origin() + ray.direction() * distance;
                    intersection.normal = Math::Normal(point - mPosition) / mRadius;
                    intersection.surfacePoint = Math::Point2D();
                    intersection.tangent = Math::Bivector(Math::Vector(), Math::Vector());
                    return true;
                }
            }

            return false;
        }

        BoundingVolume Sphere::boundingVolume(const Math::Transformation &) const
        {
            float mins[BoundingVolume::NUM_VECTORS];
            float maxes[BoundingVolume::NUM_VECTORS];

            for(int i=0; i<BoundingVolume::NUM_VECTORS; i++) {
                const Math::Vector &vector = BoundingVolume::vectors()[i];
                float x = Math::Vector(mPosition) * vector;
                float y = mRadius * vector.magnitude();

                mins[i] = x - y;
                maxes[i] = x + y;
            }

            return BoundingVolume(mins, maxes);
        }
    }
}
