#ifndef OBJECT_LIGHT_HPP
#define OBJECT_LIGHT_HPP

#include "Parse/Forwards.hpp"

#include "Object/Base.hpp"
#include "Object/Radiance.hpp"

#include <memory>

namespace Object {

class Light : public Base
{
public:
	Light(const Radiance &radiance);
	static std::unique_ptr<Light> fromAst(AST *ast);

	const Radiance &radiance() const;

protected:
	Radiance mRadiance;
};

}
#endif
