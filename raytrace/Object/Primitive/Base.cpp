#include "Object/Primitive/Base.hpp"

#include "Object/Primitive/Sphere.hpp"
#include "Object/Primitive/Box.hpp"
#include "Object/Primitive/Plane.hpp"
#include "Object/Primitive/Cone.hpp"
#include "Object/Primitive/Disk.hpp"
#include "Object/Primitive/Cylinder.hpp"
#include "Object/Primitive/Csg.hpp"

namespace Object {
namespace Primitive {

Base::Base()
{
	mTexture = 0;
}

Base::~Base()
{
	if(mTexture)
		delete mTexture;
}

Base *Base::fromAst(AST *ast)
{
	Base *primitive;
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

	for(int i=0; i<sub->numChildren; i++)
	{
		switch(sub->children[i]->type)
		{
		case AstTransform:
			primitive->transform(Math::Transformation::fromAst(sub->children[i]));
			break;
		case AstTexture:
			primitive->setTexture(Texture::fromAst(sub->children[i]));
			break;
		}
	}

	return primitive;
}

Texture *Base::texture() const 
{ 
	return mTexture; 
}

void Base::setTexture(Texture *texture)
{
	if(mTexture)
		delete mTexture;

	mTexture = texture;
}

void Base::intersect(const Trace::Ray &ray, std::vector<Trace::Intersection> &intersections) const
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