#ifndef FINISH_H
#define FINISH_H

#include "Parse/AST.h"

namespace Object {

class Finish
{
public:
	Finish();
	virtual ~Finish();

	static Finish *fromAst(AST *ast);

	float ambient() const;
	void setAmbient(float ambient);

	float diffuse() const;
	void setDiffuse(float diffuse);

	float specular() const;
	void setSpecular(float specular);

	float specularPower() const;
	void setSpecularPower(float specularPower);

	float reflection() const;
	void setReflection(float reflection);

protected:
	float mAmbient;
	float mDiffuse;
	float mSpecular;
	float mSpecularPower;
	float mReflection;
};

inline float Finish::ambient() const
{
	return mAmbient;
}

inline void Finish::setAmbient(float ambient)
{
	mAmbient = ambient;
}

inline float Finish::diffuse() const
{
	return mDiffuse;
}

inline void Finish::setDiffuse(float diffuse)
{
	mDiffuse = diffuse;
}

inline float Finish::specular() const
{
	return mSpecular;
}

inline void Finish::setSpecular(float specular)
{
	mSpecular = specular;
}

inline float Finish::specularPower() const
{
	return mSpecularPower;
}

inline void Finish::setSpecularPower(float specularPower)
{
	mSpecularPower = specularPower;
}

inline float Finish::reflection() const
{
	return mReflection;
}

inline void Finish::setReflection(float reflection)
{
	mReflection = reflection;
}

}
#endif