#include "Object/Primitive/Base.hpp"

#include "Object/Primitive/Sphere.hpp"
#include "Object/Primitive/Box.hpp"
#include "Object/Primitive/Plane.hpp"
#include "Object/Primitive/Cone.hpp"
#include "Object/Primitive/Disk.hpp"
#include "Object/Primitive/Cylinder.hpp"
#include "Object/Primitive/Csg.hpp"

#include "Object/Surface.hpp"

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
	case AstPlane:
		primitive = Plane::fromAst(sub);
		break;
	case AstBox:
		primitive = Box::fromAst(sub);
		break;
	case AstCone:
		primitive = Cone::fromAst(sub);
		break;
	case AstCylinder:
		primitive = Cylinder::fromAst(sub);
		break;
	case AstUnion:
	case AstDifference:
	case AstIntersection:
		primitive = Csg::fromAst(sub);
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

}
}