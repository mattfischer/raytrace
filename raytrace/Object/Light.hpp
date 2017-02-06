#ifndef OBJECT_LIGHT_HPP
#define OBJECT_LIGHT_HPP

#include "Parse/Forwards.hpp"

#include "Object/Base.hpp"
#include "Object/Color.hpp"

#include <memory>

namespace Object {

class Light : public Base
{
public:
	Light(const Color &color);
	static std::unique_ptr<Light> fromAst(AST *ast);

	const Color &color() const;

protected:
	Color mColor;
};

}
#endif
