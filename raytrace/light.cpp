#include "light.h"

Light::Light()
{
}

Light::Light(const Color &color)
: mColor(color)
{
}

Light::Light(const Light &c)
{
	mColor = c.mColor;
}

Light *Light::fromAst(AST *ast)
{
	Light *light = new Light();

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

Light &Light::operator=(const Light &c)
{
	mColor = c.mColor;

	return *this;
}

const Color &Light::color() const
{
	return mColor;
}

void Light::setColor(const Color &color)
{
	mColor = color;
}
