#ifndef SURFACE_ALBEDO_CHECKER_HPP
#define SURFACE_ALBEDO_CHECKER_HPP

#include "Surface/Albedo/Base.hpp"

namespace Surface {
namespace Albedo {

class Checker : public Base
{
public:
	Checker(const Object::Color &color1, const Object::Color &color2);
	virtual ~Checker();

	virtual Object::Color color(const Math::Point &point) const;

protected:
	Object::Color mColor1;
	Object::Color mColor2;
};

}
}
#endif