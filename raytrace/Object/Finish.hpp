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

	double ambient() const;
	void setAmbient(double ambient);

	double diffuse() const;
	void setDiffuse(double diffuse);

	double specular() const;
	void setSpecular(double specular);

	double specularPower() const;
	void setSpecularPower(double specularPower);

	double reflection() const;
	void setReflection(double reflection);

protected:
	double mAmbient;
	double mDiffuse;
	double mSpecular;
	double mSpecularPower;
	double mReflection;
};

}
#endif