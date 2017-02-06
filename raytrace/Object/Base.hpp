#ifndef OBJECT_BASE_HPP
#define OBJECT_BASE_HPP

#include "Math/Transformation.hpp"

namespace Object {

class Base
{
public:
	void transform(const Math::Transformation &transform);
	const Math::Transformation &transformation() const;

protected:
	Math::Transformation mTransformation;
};

}
#endif
