#ifndef OBJECT_PIGMENT_CHECKER_HPP
#define OBJECT_PIGMENT_CHECKER_HPP

#include "Object/Pigment/Base.hpp"

namespace Object {
namespace Pigment {

class Checker : public Base
{
public:
	Checker(const Color &color1, const Color &color2);
	virtual ~Checker();

	virtual Color pointColor(const Math::Point &point) const;

protected:
	Color mColor1;
	Color mColor2;
};

}
}
#endif