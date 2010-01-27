#ifndef CAMERA_H
#define CAMERA_H

#include "object.h"
#include "ray.h"

class Camera : public Object
{
public:
	Camera();
	Camera(const Camera &c);
	Camera &operator=(const Camera &c);

	static Camera *fromAst(AST *ast);

	void setFOV(double FOV);
	double FOV() const;

	void setAspectRatio(double aspectRatio);
	double aspectRatio() const;

	Ray createRay(double x, double y, double width, double height);

protected:
	void computeSpans();

	double mFOV;
	double mAspectRatio;
	double mHSpan;
	double mVSpan;
};

#endif
