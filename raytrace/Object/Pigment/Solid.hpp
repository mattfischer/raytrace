#ifndef SOLID_H
#define SOLID_H

#include "Object/Pigment/Base.hpp"

namespace Object {
namespace Pigment {

class Solid : public Base
{
public:
	Solid(const Color &color);
	virtual ~Solid();

	virtual Color pointColor(const Math::Point &point) const;

protected:
	Color mColor;
};

}
}
#endif