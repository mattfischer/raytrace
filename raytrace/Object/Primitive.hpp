#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "Object/Object.hpp"
#include "Trace/Intersection.hpp"
#include "Math/Ray.hpp"
#include "Math/Point.hpp"
#include "Object/Texture.hpp"

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

	void intersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	bool inside(const Math::Point &point) const;

protected:
	Texture *mTexture;

	virtual void doIntersect(const Math::Ray &ray, std::vector<Trace::Intersection> &intersections) const = 0;
	virtual bool doInside(const Math::Point &point) const = 0;
};

}
#endif