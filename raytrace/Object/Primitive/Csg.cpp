#include "Object/Primitive/Csg.hpp"

namespace Object {
namespace Primitive {

Csg::Csg() :
	mPrimitive1(0),
	mPrimitive2(0)
{
}

Csg *Csg::fromAst(AST *ast)
{
	Csg *csg = new Csg();

	switch(ast->type)
	{
	case AstUnion:
		csg->setType(TypeUnion);
		break;

	case AstDifference:
		csg->setType(TypeDifference);
		break;

	case AstIntersection:
		csg->setType(TypeIntersection);
		break;
	}

	for(int i=0; i<ast->numChildren; i++)
	{
		switch(ast->children[i]->type)
		{
		case AstPrimitive:
			if(csg->primitive1() == 0)
			{
				csg->setPrimitive1(Primitive::Base::fromAst(ast->children[i]));
			}
			else
			{
				csg->setPrimitive2(Primitive::Base::fromAst(ast->children[i]));
			}
			break;

		case AstTransform:
			csg->transform(Math::Transformation::fromAst(ast->children[i]));
			break;
		}
	}

	return csg;
}

Base *Csg::primitive1()
{
	return mPrimitive1;
}

void Csg::setPrimitive1(Base *primitive1)
{
	mPrimitive1 = primitive1;
}

Base *Csg::primitive2()
{
	return mPrimitive2;
}

void Csg::setPrimitive2(Base *primitive2)
{
	mPrimitive2 = primitive2;
}

Csg::Type Csg::type() const
{
	return mType;
}

void Csg::setType(Type type)
{
	mType = type;
}

bool Csg::doInside(const Math::Point &point) const
{
	switch(mType)
	{
	case TypeUnion:
		return mPrimitive1->inside(point) || mPrimitive2->inside(point);
	case TypeIntersection:
		return mPrimitive1->inside(point) && mPrimitive2->inside(point);
	case TypeDifference:
		return mPrimitive1->inside(point) && !mPrimitive2->inside(point);
	}

	return false;
}

void Csg::doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const
{
	std::vector<Trace::Intersection> localIntersections;

	mPrimitive1->intersect(ray, localIntersections);
	mPrimitive2->intersect(ray, localIntersections);

	for(int i=0; i<localIntersections.size(); i++)
	{
		bool add = false;

		switch(mType)
		{
		case TypeUnion:
			if(localIntersections[i].primitive() == mPrimitive1 && !mPrimitive2->inside(localIntersections[i].point()) ||
			   localIntersections[i].primitive() == mPrimitive2 && !mPrimitive1->inside(localIntersections[i].point()))
			{
				add = true;
			}
			break;

		case TypeIntersection:
			if(localIntersections[i].primitive() == mPrimitive1 && mPrimitive2->inside(localIntersections[i].point()) ||
			   localIntersections[i].primitive() == mPrimitive2 && mPrimitive1->inside(localIntersections[i].point()))
			{
				add = true;
			}
			break;

		case TypeDifference:
			if(localIntersections[i].primitive() == mPrimitive1 && !mPrimitive2->inside(localIntersections[i].point()) ||
			   localIntersections[i].primitive() == mPrimitive2 && mPrimitive1->inside(localIntersections[i].point()))
			{
				add = true;
			}
			break;
		}

		if(add)
		{
			localIntersections[i].transform(transformation());
			intersections.push_back(localIntersections[i]);
		}
	}
}

}
}