#include "Object/Light.hpp"

namespace Object {

std::unique_ptr<Light> Light::fromAst(AST *ast)
{
	std::unique_ptr<Light> light = std::make_unique<Light>();

	for(int i=0; i<ast->numChildren; i++)
	{
		switch(ast->children[i]->type)
		{
		case AstColor:
			light->setColor(ast->children[i]->data._vector);
			break;

		case AstTransform:
			light->transform(Math::Transformation::fromAst(ast->children[i]));
			break;
		}
	}

	return light;
}

const Color &Light::color() const
{
	return mColor;
}

void Light::setColor(const Color &color)
{
	mColor = color;
}

}