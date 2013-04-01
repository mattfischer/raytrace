#include "Object/Texture.hpp"

#include "Object/Pigment/Solid.hpp"

namespace Object {

Texture::Texture(Pigment::Base *pigment, Finish *finish)
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

Texture *Texture::fromAst(AST *ast)
{
	Texture *texture = new Texture();

	for(int i=0; i<ast->numChildren; i++)
	{
		switch(ast->children[i]->type)
		{
		case AstPigment:
			texture->setPigment(Pigment::Base::fromAst(ast->children[i]));
			break;
		case AstFinish:
			texture->setFinish(Finish::fromAst(ast->children[i]));
			break;
		}
	}

	if(texture->pigment() == 0)
	{
		texture->setPigment(new Pigment::Solid(Color(0,0,0)));
	}

	if(texture->finish() == 0)
	{
		texture->setFinish(new Finish());
	}

	return texture;
}

void Texture::transform(const Math::Transformation &transformation)
{
	mTransformation = transformation.transformTransformation(mTransformation);
}

const Math::Transformation &Texture::transformation() const
{
	return mTransformation;
}

Pigment::Base *Texture::pigment() const
{
	return mPigment;
}

void Texture::setPigment(Pigment::Base *pigment)
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

Color Texture::pointColor(const Math::Point &point) const
{
	return mPigment->pointColor(mTransformation.inverseTransformPoint(point));
}

}