#ifndef OBJECT_H
#define OBJECT_H

#include "Math/Transformation.hpp"

namespace Object {

class Object
{
public:
	Object();
	virtual ~Object();

	void transform(const Math::Transformation &transform);
	const Math::Transformation &transformation() const;

protected:
	Math::Transformation mTransformation;
};

}
#endif
