#include "finish.h"

#define AMBIENT_DEFAULT 0.1
#define DIFFUSE_DEFAULT 0.8
#define SPECULAR_DEFAULT 0.0
#define SPECULARPOWER_DEFAULT 50
#define REFLECTION_DEFAULT 0

Finish::Finish()
{
	mAmbient = AMBIENT_DEFAULT;
	mDiffuse = DIFFUSE_DEFAULT;
	mSpecular = SPECULAR_DEFAULT;
	mSpecularPower = SPECULARPOWER_DEFAULT;
	mReflection = REFLECTION_DEFAULT;
}

Finish::~Finish()
{
}

Finish *Finish::fromAst(AST *ast)
{
	Finish *finish = new Finish();

	for(int i=0; i<ast->numChildren; i++)
	{
		switch(ast->children[i]->type)
		{
		case AstAmbient:
			finish->setAmbient(ast->children[i]->data._double);
			break;

		case AstDiffuse:
			finish->setDiffuse(ast->children[i]->data._double);
			break;

		case AstSpecular:
			finish->setSpecular(ast->children[i]->data._double);
			break;

		case AstSpecularPower:
			finish->setSpecularPower(ast->children[i]->data._double);
			break;

		case AstReflection:
			finish->setReflection(ast->children[i]->data._double);
			break;
		}
	}

	return finish;
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

double Finish::reflection() const
{
	return mReflection;
}

void Finish::setReflection(double reflection)
{
	mReflection = reflection;
}