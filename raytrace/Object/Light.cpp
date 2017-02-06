#include "Object/Light.hpp"

namespace Object {

Light::Light(const Color &color)
	: mColor(color)
{
}

std::unique_ptr<Light> Light::fromAst(AST *ast)
{
	Color color;
	for (int i = 0; i < ast->numChildren; i++)
	{
		switch (ast->children[i]->type)
		{
		case AstColor:
			color = ast->children[i]->data._vector;
			break;
		}
	}

	std::unique_ptr<Light> light = std::make_unique<Light>(color);

	for (int i = 0; i < ast->numChildren; i++)
	{
		switch (ast->children[i]->type)
		{
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

}