#include "finish.h"

#define AMBIENT_DEFAULT 0.1
#define DIFFUSE_DEFAULT 0.8
#define SPECULAR_DEFAULT 0.0
#define SPECULARPOWER_DEFAULT 50

Finish::Finish()
{
	mAmbient = AMBIENT_DEFAULT;
	mDiffuse = DIFFUSE_DEFAULT;
	mSpecular = SPECULAR_DEFAULT;
	mSpecularPower = SPECULARPOWER_DEFAULT;
}

Finish::~Finish()
{
}

double Finish::ambient() const
{
	return mAmbient;
}

void Finish::setAmbient(double ambient)
{
	mAmbient = ambient;
}

double Finish::diffuse() const
{
	return mDiffuse;
}

void Finish::setDiffuse(double diffuse)
{
	mDiffuse = diffuse;
}

double Finish::specular() const
{
	return mSpecular;
}

void Finish::setSpecular(double specular)
{
	mSpecular = specular;
}

double Finish::specularPower() const
{
	return mSpecularPower;
}

void Finish::setSpecularPower(double specularPower)
{
	mSpecularPower = specularPower;
}