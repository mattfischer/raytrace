#include "Object/Finish.hpp"

#define AMBIENT_DEFAULT 0
#define DIFFUSE_DEFAULT 0.8
#define SPECULAR_DEFAULT 0.0
#define SPECULARPOWER_DEFAULT 50
#define REFLECTION_DEFAULT 0

namespace Object {

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
			finish->setAmbient(ast->children[i]->data._float);
			break;

		case AstDiffuse:
			finish->setDiffuse(ast->children[i]->data._float);
			break;

		case AstSpecular:
			finish->setSpecular(ast->children[i]->data._float);
			break;

		case AstSpecularPower:
			finish->setSpecularPower(ast->children[i]->data._float);
			break;

		case AstReflection:
			finish->setReflection(ast->children[i]->data._float);
			break;
		}
	}

	return finish;
}

float Finish::ambient() const
{
	return mAmbient;
}

void Finish::setAmbient(float ambient)
{
	mAmbient = ambient;
}

float Finish::diffuse() const
{
	return mDiffuse;
}

void Finish::setDiffuse(float diffuse)
{
	mDiffuse = diffuse;
}

float Finish::specular() const
{
	return mSpecular;
}

void Finish::setSpecular(float specular)
{
	mSpecular = specular;
}

float Finish::specularPower() const
{
	return mSpecularPower;
}

void Finish::setSpecularPower(float specularPower)
{
	mSpecularPower = specularPower;
}

float Finish::reflection() const
{
	return mReflection;
}

void Finish::setReflection(float reflection)
{
	mReflection = reflection;
}

}