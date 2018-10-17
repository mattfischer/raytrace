#include "Object/Light.hpp"

#include "Math/Vector.hpp"
#include "Math/Transformation.hpp"

namespace Object {

Light::Light(const Math::Point &position, const Object::Radiance &radiance)
	: mPosition(position), mRadiance(radiance)
{
}

std::unique_ptr<Light> Light::fromAst(AST *ast)
{
	Math::Point position(ast->children[0]->children[0]->data._vector);
	Object::Radiance radiance(ast->children[0]->children[1]->data._vector);
	std::unique_ptr<Light> light = std::make_unique<Light>(position, radiance);

	return light;
}

const Math::Point &Light::position() const
{
	return mPosition;
}

const Object::Radiance &Light::radiance() const
{
	return mRadiance;
}

}