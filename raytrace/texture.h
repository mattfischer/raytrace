#ifndef TEXTURE_H
#define TEXTURE_H

#include "transformation.h"
#include "pigment.h"
#include "finish.h"
#include "ast.h"

class Texture
{
public:
	Texture(Pigment *pigment = 0, Finish *finish = 0);
	virtual ~Texture();

	static Texture *fromAst(AST *ast);

	void transform(const Transformation &transform);
	const Transformation &transformation() const;

	Pigment *pigment() const;
	void setPigment(Pigment *pigment);

	Finish *finish() const;
	void setFinish(Finish *finish);

	Color pointColor(const Vector &point) const;

protected:
	Transformation mTransformation;

	Pigment *mPigment;
	Finish *mFinish;
};

#endif