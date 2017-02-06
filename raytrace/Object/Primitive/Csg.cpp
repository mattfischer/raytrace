#include "Object/Primitive/Csg.hpp"

namespace Object {
namespace Primitive {

Csg::Csg()
{
}

std::unique_ptr<Csg> Csg::fromAst(AST *ast)
{
	std::unique_ptr<Csg> csg = std::make_unique<Csg>();

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
			if(!csg->mPrimitive1)
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

const Base &Csg::primitive1() const
{
	return *mPrimitive1;
}

void Csg::setPrimitive1(std::unique_ptr<Base> &&primitive1)
{
	mPrimitive1 = std::move(primitive1);
}

const Base &Csg::primitive2() const
{
	return *mPrimitive2;
}

void Csg::setPrimitive2(std::unique_ptr<Base> &&primitive2)
{
	mPrimitive2 = std::move(primitive2);
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

void Csg::doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const
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
			if(localIntersections[i].primitive() == mPrimitive1.get() && !mPrimitive2->inside(localIntersections[i].point()) ||
			   localIntersections[i].primitive() == mPrimitive2.get() && !mPrimitive1->inside(localIntersections[i].point()))
			{
				add = true;
			}
			break;

		case TypeIntersection:
			if(localIntersections[i].primitive() == mPrimitive1.get() && mPrimitive2->inside(localIntersections[i].point()) ||
			   localIntersections[i].primitive() == mPrimitive2.get() && mPrimitive1->inside(localIntersections[i].point()))
			{
				add = true;
			}
			break;

		case TypeDifference:
			if(localIntersections[i].primitive() == mPrimitive1.get() && !mPrimitive2->inside(localIntersections[i].point()) ||
			   localIntersections[i].primitive() == mPrimitive2.get() && mPrimitive1->inside(localIntersections[i].point()))
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