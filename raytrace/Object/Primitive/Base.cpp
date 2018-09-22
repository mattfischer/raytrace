#include "Object/Primitive/Base.hpp"

#include "Object/Primitive/Sphere.hpp"
#include "Object/Primitive/Box.hpp"
#include "Object/Primitive/Quad.hpp"

#include "Object/Surface.hpp"

#include <algorithm>

namespace Object {
namespace Primitive {

std::unique_ptr<Base> Base::fromAst(AST *ast)
{
	std::unique_ptr<Base> primitive;
	AST *sub = ast->children[0];

	switch(sub->type)
	{
	case AstSphere:
		primitive = Sphere::fromAst(sub);
		break;
	case AstBox:
		primitive = Box::fromAst(sub);
		break;
	case AstQuad:
		primitive = Quad::fromAst(sub);
		break;
	}

	parseAstCommon(*primitive, sub);

	return primitive;
}

void Base::parseAstCommon(Base &primitive, AST *ast)
{
	for (int i = 0; i<ast->numChildren; i++)
	{
		switch (ast->children[i]->type)
		{
		case AstTransform:
			primitive.transform(Math::Transformation::fromAst(ast->children[i]));
			break;
		case AstSurface:
			primitive.setSurface(Surface::fromAst(ast->children[i]));
			break;
		}
	}
}

const Surface &Base::surface() const 
{ 
	return *mSurface;
}

void Base::setSurface(std::unique_ptr<Surface> &&surface)
{
	mSurface = std::move(surface);
}

void Base::intersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const
{
	Trace::Ray transformedRay = mTransformation.inverse() * ray;

	doIntersect(transformedRay, intersections);
}

bool Base::inside(const Math::Point &point) const
{
	Math::Point transformedPoint = mTransformation.inverse() * point;

	return doInside(transformedPoint);
}

void Base::sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const
{
	Math::Point objectPoint;
	Math::Vector objectDu;
	Math::Vector objectDv;
	Math::Normal objectNormal;

	doSample(u, v, objectPoint, objectDu, objectDv, objectNormal);

	point = mTransformation * objectPoint;
	du = mTransformation * objectDu;
	dv = mTransformation * objectDv;
	normal = mTransformation * objectNormal;
}

bool Base::canSample() const
{
	return false;
}

void Base::doSample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const
{
}

void Base::doTransform()
{
	std::vector<Math::Vector> vectors;
	for (const Math::Vector &vector : BoundingVolume::vectors()) {
		vectors.push_back(mTransformation.inverse() * vector);
	}

	BoundingVolume objectVolume = doBoundingVolume(vectors);
	mBoundingVolume = objectVolume.translate(Math::Vector(mTransformation.origin()));
}

}
}