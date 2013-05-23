#ifndef LIGHT_H
#define LIGHT_H

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

inline const Color &Light::color() const
{
	return mColor;
}

inline void Light::setColor(const Color &color)
{
	mColor = color;
}

}
#endif
