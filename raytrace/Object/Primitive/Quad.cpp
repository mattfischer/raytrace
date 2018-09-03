#include "Object/Primitive/Quad.hpp"

#include "Object/Primitive/Plane.hpp"

#include <math.h>

#define EPSILON .01

namespace Object {
	namespace Primitive {

		std::unique_ptr<Quad> Quad::fromAst(AST *ast)
		{
			std::unique_ptr<Quad> quad = std::make_unique<Quad>();

			Math::Vector position(ast->children[0]->children[0]->data._vector);
			float width = ast->children[0]->children[1]->data._float;
			float height = ast->children[0]->children[2]->data._float;
			Math::Vector size(width, 0, height);

			quad->transform(Math::Transformation::translate(position + size / 2));
			size.setY(2);
			quad->transform(Math::Transformation::scale(size / 2));

			parseAstCommon(*quad, ast->children[1]);

			return quad;
		}

		bool Quad::canSample() const
		{
			return true;
		}

		void Quad::doIntersect(const Trace::Ray &ray, std::vector<Trace::Intersection> &intersections) const
		{
			float t = -Math::Vector(ray.origin()) * ray.direction() / ray.direction().magnitude2();
			Math::Point v = ray.origin() + ray.direction() * t;
			if (Math::Vector(v).magnitude2()>2) return;

			Trace::Intersection intersection = Plane::intersectPlane(this, ray, Math::Normal(0, 1, 0), 0);
			Math::Point point;

			if (intersection.valid())
			{
				point = intersection.objectPoint();

				if (abs(point.x()) <= 1 && abs(point.z()) <= 1)
				{
					intersections.push_back(intersection);
				}
			}
		}

		bool Quad::doInside(const Math::Point &point) const
		{
			return false;
		}

		BoundingSphere Quad::doBoundingSphere() const
		{
			return BoundingSphere(Math::Point(0, 0, 0), std::sqrt(2));
		}

		void Quad::doSample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const
		{
			point = Math::Point(u * 2 - 1, 0, v * 2 - 1);
			du = Math::Vector(2, 0, 0);
			dv = Math::Vector(0, 0, 2);
			normal = Math::Normal(0, -1, 0);
		}
	}
}