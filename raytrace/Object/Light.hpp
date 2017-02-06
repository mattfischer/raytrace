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
	static std::unique_ptr<Light> fromAst(AST *ast);

	const Color &color() const;
	void setColor(const Color &color);

protected:
	Color mColor;
};

}
#endif
