#include "Object/Light.hpp"

namespace Object {

Light::Light(const Radiance &radiance)
	: mRadiance(radiance)
{
}

std::unique_ptr<Light> Light::fromAst(AST *ast)
{
	Radiance radiance;
	for (int i = 0; i < ast->numChildren; i++)
	{
		switch (ast->children[i]->type)
		{
		case AstRadiance:
			radiance = ast->children[i]->data._vector;
			break;
		}
	}

	std::unique_ptr<Light> light = std::make_unique<Light>(radiance);

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

const Radiance &Light::radiance() const
{
	return mRadiance;
}

}