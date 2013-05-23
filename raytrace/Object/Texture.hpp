#ifndef TEXTURE_H
#define TEXTURE_H

#include "Math/Transformation.hpp"
#include "Object/Pigment/Base.hpp"
#include "Object/Finish.hpp"
#include "Parse/AST.h"

namespace Object {

class Texture
{
public:
	Texture(Pigment::Base *pigment = 0, Finish *finish = 0);
	virtual ~Texture();

	static Texture *fromAst(AST *ast);

	void transform(const Math::Transformation &transform);
	const Math::Transformation &transformation() const;

	Pigment::Base *pigment() const;
	void setPigment(Pigment::Base *pigment);

	Finish *finish() const;
	void setFinish(Finish *finish);

	Color pointColor(const Math::Point &point) const;

protected:
	Math::Transformation mTransformation;

	Pigment::Base *mPigment;
	Finish *mFinish;
};

inline const Math::Transformation &Texture::transformation() const
{
	return mTransformation;
}

inline Pigment::Base *Texture::pigment() const
{
	return mPigment;
}

inline void Texture::setPigment(Pigment::Base *pigment)
{
	if(mPigment)
		delete mPigment;

	mPigment = pigment;
}

inline Finish *Texture::finish() const
{
	return mFinish;
}

inline void Texture::setFinish(Finish *finish)
{
	if(mFinish)
		delete mFinish;

	mFinish = finish;
}

}
#endif