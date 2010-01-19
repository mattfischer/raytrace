#ifndef CAMERA_H
#define CAMERA_H

#include "object.h"
#include "ray.h"

class Camera : public Object
{
public:
	Camera(double hFOV, double aspectRatio);
	Camera(const Camera &c);
	Camera &operator=(const Camera &c);

	Ray createRay(double x, double y, double width, double height);

protected:
	double mHSpan;
	double mVSpan;
};

#endif
