#include "Object/Primitive.hpp"

namespace Object {

std::unique_ptr<Primitive> Primitive::fromAst(AST *ast)
{
	AST *sub = ast->children[0];
	std::unique_ptr<Shape::Base> shape = Shape::Base::fromAst(sub);

	std::unique_ptr<Surface> surface;
	Math::Transformation transformation;

	AST *subsub = sub->children[1];
	for (int i = 0; i<subsub->numChildren; i++)
	{
		switch (subsub->children[i]->type)
		{
		case AstTransform:
			transformation = transformation * Math::Transformation::fromAst(subsub->children[i]);
			break;
		
		case AstSurface:
			surface = Surface::fromAst(subsub->children[i]);
			break;
		}
	}

	return std::make_unique<Primitive>(std::move(shape), std::move(surface), transformation);
}

Primitive::Primitive(std::unique_ptr<Shape::Base> shape, std::unique_ptr<Surface> surface, const Math::Transformation &transformation)
	: mShape(std::move(shape)), mSurface(std::move(surface)), mTransformation(transformation)
{
	mBoundingVolume = mShape->boundingVolume(mTransformation);
}

const Math::Transformation &Primitive::transformation() const
{
	return mTransformation;
}

const Shape::Base &Primitive::shape() const
{
	return *mShape;
}

const Surface &Primitive::surface() const
{
	return *mSurface;
}

const BoundingVolume &Primitive::boundingVolume() const
{
	return mBoundingVolume;
}

float Primitive::intersect(const Math::Ray &ray, Math::Normal &normal) const
{
	Math::Ray transformedRay = mTransformation.inverse() * ray;
	Math::Normal transformedNormal;
	float distance = mShape->intersect(transformedRay, transformedNormal);

	if (distance != FLT_MAX) {
		normal = (transformation() * transformedNormal).normalize();
	}

	return distance;
}

bool Primitive::sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const
{
	Math::Point objectPoint;
	Math::Vector objectDu;
	Math::Vector objectDv;
	Math::Normal objectNormal;

	if (mShape->sample(u, v, objectPoint, objectDu, objectDv, objectNormal))
	{
		point = mTransformation * objectPoint;
		du = mTransformation * objectDu;
		dv = mTransformation * objectDv;
		normal = mTransformation * objectNormal;

		return true;
	}
	else {
		return false;
	}
}

}