#ifndef OBJECT_LIGHT_HPP
#define OBJECT_LIGHT_HPP

#include "Object/Base.hpp"
#include "Object/Radiance.hpp"

#include <memory>

namespace Object {

class Light : public Object::Base
{
public:
	Light(const Object::Radiance &radiance);

	static std::unique_ptr<Light> fromAst(AST *ast);

	const Object::Radiance &radiance() const;

private:
	Object::Radiance mRadiance;
};
}
#endif