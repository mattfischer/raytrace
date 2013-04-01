#include "primitive.h"

#include "sphere.h"
#include "box.h"
#include "plane.h"
#include "cone.h"
#include "disk.h"
#include "cylinder.h"
#include "csg.h"

Primitive::Primitive()
{
	mTexture = 0;
}

Primitive::~Primitive()
{
	if(mTexture)
		delete mTexture;
}

Primitive *Primitive::fromAst(AST *ast)
{
	Primitive *primitive;
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

Texture *Primitive::texture() const 
{ 
	return mTexture; 
}

void Primitive::setTexture(Texture *texture)
{
	if(mTexture)
		delete mTexture;

	mTexture = texture;
}

void Primitive::intersect(const Math::Ray &ray, std::vector<Intersection> &intersections) const
{
	Math::Ray transformedRay = mTransformation.inverseTransformRay(ray);

	doIntersect(transformedRay, intersections);
}

bool Primitive::inside(const Math::Vector &point) const
{
	Math::Vector transformedPoint = mTransformation.inverseTransformPoint(point);

	return doInside(transformedPoint);
}