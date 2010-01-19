#ifndef OBJECT_H
#define OBJECT_H

#include "transformation.h"

class Object
{
public:
	Object();
	virtual ~Object();

	void transform(const Transformation &transform);
	const Transformation &transformation() const;

protected:
	Transformation mTransformation;
};

#endif
