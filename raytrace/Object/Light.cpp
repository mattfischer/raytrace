#include "Object/Light.hpp"

#include "Math/Vector.hpp"
#include "Math/Transformation.hpp"

namespace Object {

Light::Light(const Object::Radiance &radiance)
	: mRadiance(radiance)
{
}

std::unique_ptr<Light> Light::fromAst(AST *ast)
{
	Math::Vector position(ast->children[0]->children[0]->data._vector);
	Object::Radiance radiance(ast->children[0]->children[1]->data._vector);
	std::unique_ptr<Light> light = std::make_unique<Light>(radiance);
	float radius = ast->children[0]->children[1]->data._float;
	light->transform(Math::Transformation::translate(position));

	for (int i = 0; i<ast->numChildren; i++)
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

const Object::Radiance &Light::radiance() const
{
	return mRadiance;
}

}