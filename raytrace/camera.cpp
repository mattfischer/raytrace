#include "camera.h"

#include <math.h>
#include <stdio.h>

static double rad(double deg)
{
	return deg * 3.14 / 180.0;
}

Camera::Camera(double hFOV, double aspectRatio)
{
	mHSpan = 2 * sin(rad(hFOV / 2));
	mVSpan = mHSpan * aspectRatio;
}

Camera::Camera(const Camera &c)
{
	mHSpan = c.mHSpan;
	mVSpan = c.mVSpan;
}

Camera &Camera::operator=(const Camera &c)
{
	mHSpan = c.mHSpan;
	mVSpan = c.mVSpan;

	return *this;
}

Ray Camera::createRay(double x, double y, double width, double height)
{
	double rayX, rayY;

	rayX = mHSpan * x / width - mHSpan / 2;
	rayY = mVSpan / 2 - mVSpan * y / height;

	return Ray(transformation().origin(), transformation().transformDirection(Vector(rayX, rayY, 1).normalize()));
}

