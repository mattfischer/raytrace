#include "Object/Primitive/Quad.hpp"

#include <math.h>
#include <algorithm>

namespace Object {
	namespace Primitive {

		std::unique_ptr<Quad> Quad::fromAst(AST *ast)
		{
			Math::Point position(ast->children[0]->children[0]->data._vector);
			Math::Vector side1(ast->children[0]->children[1]->data._vector);
			Math::Vector side2(ast->children[0]->children[2]->data._vector);

			std::unique_ptr<Quad> quad = std::make_unique<Quad>(position, side1, side2);

			parseAstCommon(*quad, ast->children[1]);

			return quad;
		}

		Quad::Quad(const Math::Point &position, const Math::Vector &side1, const Math::Vector &side2)
			: mPosition(position), mSide1(side1), mSide2(side2)
		{
			mNormal = Math::Normal(mSide1 % mSide2).normalize();
		}

		float Quad::doIntersect(const Math::Ray &ray, Math::Normal &normal) const
		{
			float distance = ((ray.origin() - mPosition) * mNormal) / (ray.direction() * -mNormal);
			if (distance >= 0) {
				Math::Point point = ray.origin() + ray.direction() * distance;
				float u = (point - mPosition) * mSide1 / mSide1.magnitude2();
				float v = (point - mPosition) * mSide2 / mSide2.magnitude2();
				if (u >= 0 && u <= 1 && v >= 0 && v <= 1)
				{
					normal = mNormal;
					return distance;
				}
			}

			return FLT_MAX;
		}

		bool Quad::doInside(const Math::Point &point) const
		{
			return false;
		}

		void Quad::doSample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const
		{
			point = mPosition + mSide1 * u + mSide2 * v;
			du = mSide1;
			dv = mSide2;
			normal = mNormal;
		}

		BoundingVolume Quad::doBoundingVolume() const
		{
			Math::Point points[] = { mPosition, mPosition + mSide1, mPosition + mSide2, mPosition + mSide1 + mSide2 };

			BoundingVolume volume;
			for (const Math::Point &point : points) {
				volume.expand(transformation() * point);
			}

			return volume;
		}

	}
}