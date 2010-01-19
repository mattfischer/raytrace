#ifndef LIGHT_H
#define LIGHT_H

#include "object.h"
#include "color.h"

class Light : public Object
{
public:
	Light(const Color &color);
	Light(const Light &c);
	Light &operator=(const Light &c);

	const Color &color() const;
	void setColor(const Color &color);

protected:
	Color mColor;
};

#endif
