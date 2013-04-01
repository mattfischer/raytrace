#ifndef TEXTURE_H
#define TEXTURE_H

#include "Math/Transformation.hpp"
#include "pigment.h"
#include "finish.h"
#include "ast.h"

class Texture
{
public:
	Texture(Pigment *pigment = 0, Finish *finish = 0);
	virtual ~Texture();

	static Texture *fromAst(AST *ast);

	void transform(const Math::Transformation &transform);
	const Math::Transformation &transformation() const;

	Pigment *pigment() const;
	void setPigment(Pigment *pigment);

	Finish *finish() const;
	void setFinish(Finish *finish);

	Color pointColor(const Math::Vector &point) const;

protected:
	Math::Transformation mTransformation;

	Pigment *mPigment;
	Finish *mFinish;
};

#endif