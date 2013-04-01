#ifndef LIGHT_H
#define LIGHT_H

#include "Object/Object.hpp"
#include "color.h"

namespace Object {

class Light : public Object
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
