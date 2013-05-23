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

}
#endif