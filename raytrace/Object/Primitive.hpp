#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "Object/Object.hpp"
#include "intersection.h"
#include "Math/Ray.hpp"
#include "texture.h"

#include <vector>

namespace Object {

class Primitive : public Object
{
public:
	Primitive();
	virtual ~Primitive();

	static Primitive *fromAst(AST *ast);

	Texture *texture() const;
	void setTexture(Texture *texture);

	void intersect(const Math::Ray &ray, std::vector<Intersection> &intersections) const;
	bool inside(const Math::Vector &point) const;

protected:
	Texture *mTexture;

	virtual void doIntersect(const Math::Ray &ray, std::vector<Intersection> &intersections) const = 0;
	virtual bool doInside(const Math::Vector &point) const = 0;
};

}
#endif