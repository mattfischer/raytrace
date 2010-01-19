#include "texture.h"

Texture::Texture(Pigment *pigment, Finish *finish)
{
	mPigment = pigment;
	mFinish = finish;
}

Texture::~Texture()
{
	if(mPigment)
		delete mPigment;

	if(mFinish)
		delete mFinish;
}

void Texture::transform(const Transformation &transformation)
{
	mTransformation = transformation.transformTransformation(mTransformation);
}

const Transformation &Texture::transformation() const
{
	return mTransformation;
}

Pigment *Texture::pigment() const
{
	return mPigment;
}

void Texture::setPigment(Pigment *pigment)
{
	if(mPigment)
		delete mPigment;

	mPigment = pigment;
}

Finish *Texture::finish() const
{
	return mFinish;
}

void Texture::setFinish(Finish *finish)
{
	if(mFinish)
		delete mFinish;

	mFinish = finish;
}

Color Texture::pointColor(const Vector &point) const
{
	return mPigment->pointColor(mTransformation.inverseTransformPoint(point));
}