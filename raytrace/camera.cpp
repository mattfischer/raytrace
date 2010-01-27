#include "camera.h"

#include <math.h>
#include <stdio.h>

static double rad(double deg)
{
	return deg * 3.14 / 180.0;
}

#define DEFAULT_FOV 45
#define DEFAULT_ASPECT_RATIO 

Camera::Camera()
{
	mFOV = DEFAULT_FOV;
	mAspectRatio = 1;

	computeSpans();
}

Camera *Camera::fromAst(AST *ast)
{
	Camera *camera = new Camera();

	for(int i=0; i<ast->numChildren; i++)
	{
		switch(ast->children[i]->type)
		{
		case AstTransform:
			camera->transform(Transformation::fromAst(ast->children[i]));
			break;
		}
	}

	return camera;
}

void Camera::setFOV(double FOV)
{
	mFOV = FOV;

	computeSpans();
}

double Camera::FOV() const
{
	return mFOV;
}

void Camera::setAspectRatio(double aspectRatio)
{
	mAspectRatio = aspectRatio;

	computeSpans();
}

double Camera::aspectRatio() const
{
	return mAspectRatio;
}

Camera::Camera(const Camera &c)
{
	mFOV = c.mFOV;
	mAspectRatio = c.mAspectRatio;
	mHSpan = c.mHSpan;
	mVSpan = c.mVSpan;
}

Camera &Camera::operator=(const Camera &c)
{
	mFOV = c.mFOV;
	mAspectRatio = c.mAspectRatio;
	mHSpan = c.mHSpan;
	mVSpan = c.mVSpan;

	return *this;
}

void Camera::computeSpans()
{
	mHSpan = 2 * sin(rad(mFOV / 2));
	mVSpan = mHSpan * mAspectRatio;
}

Ray Camera::createRay(double x, double y, double width, double height)
{
	double rayX, rayY;

	rayX = mHSpan * x / width - mHSpan / 2;
	rayY = mVSpan / 2 - mVSpan * y / height;

	return Ray(transformation().origin(), transformation().transformDirection(Vector(rayX, rayY, 1).normalize()));
}

