#include "Object/Primitive.hpp"

#include "Object/Shape/Transformed.hpp"

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

		if (!transformation.matrix().identity()) {
			shape = std::make_unique<Shape::Transformed>(std::move(shape), transformation);
		}

		return std::make_unique<Primitive>(std::move(shape), std::move(surface));
	}

	Primitive::Primitive(std::unique_ptr<Shape::Base> shape, std::unique_ptr<Surface> surface)
		: mShape(std::move(shape)), mSurface(std::move(surface))
	{
		mBoundingVolume = mShape->boundingVolume(Math::Transformation());
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
}