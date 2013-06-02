#ifndef OBJECT_LIGHT_HPP
#define OBJECT_LIGHT_HPP

#include "Parse/Forwards.hpp"

#include "Object/Base.hpp"
#include "Object/Color.hpp"

namespace Object {

class Light : public Base
{
public:
	Light();
	Light(const Color &color);
	Light(const Light &c);
	Light &operator=(const Light &c);

	static Light *fromAst(AST *ast);

	const Color &color() const;
	void setColor(const Color &color);

protected:
	Color mColor;
};

}
#endif
