#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "object.h"
#include "intersection.h"
#include "ray.h"
#include "texture.h"

#include <vector>

class Primitive : public Object
{
public:
	Primitive();
	virtual ~Primitive();

	static Primitive *fromAst(AST *ast);

	Texture *texture() const;
	void setTexture(Texture *texture);

	void intersect(const Ray &ray, std::vector<Intersection> &intersections) const;
	bool inside(const Vector &point) const;

protected:
	Texture *mTexture;

	virtual void doIntersect(const Ray &ray, std::vector<Intersection> &intersections) const = 0;
	virtual bool doInside(const Vector &point) const = 0;
};
#endif