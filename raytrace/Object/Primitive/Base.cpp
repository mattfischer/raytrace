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
	primitive->computeBoundingVolume();

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

Intersection Base::intersect(const Math::Ray &ray) const
{
	Math::Ray transformedRay = mTransformation.inverse() * ray;

	Math::Normal normal;
	float distance = doIntersect(transformedRay, normal);

	if (distance == FLT_MAX) {
		return Intersection();
	}
	else {
		return Intersection(this, transformedRay, distance, normal, transformedRay.origin() + transformedRay.direction() * distance);
	}
}

bool Base::inside(const Math::Point &point) const
{
	Math::Point transformedPoint = mTransformation.inverse() * point;

	return doInside(transformedPoint);
}

const BoundingVolume &Base::boundingVolume() const
{
	return mBoundingVolume;
}

void Base::computeBoundingVolume()
{
	mBoundingVolume = doBoundingVolume();
}

}
}