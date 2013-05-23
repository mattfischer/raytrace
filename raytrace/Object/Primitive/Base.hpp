#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "Object/Base.hpp"
#include "Object/Texture.hpp"
#include "Trace/Intersection.hpp"
#include "Trace/Ray.hpp"
#include "Math/Point.hpp"

#include <vector>

namespace Object {
namespace Primitive {

class Base : public Object::Base
{
public:
	Base();
	virtual ~Base();

	static Base *fromAst(AST *ast);

	Texture *texture() const;
	void setTexture(Texture *texture);

	void intersect(const Trace::Ray &ray, std::vector<Trace::Intersection> &intersections) const;
	bool inside(const Math::Point &point) const;

protected:
	Texture *mTexture;

	virtual void doIntersect(const Trace::Ray &ray, std::vector<Trace::Intersection> &intersections) const = 0;
	virtual bool doInside(const Math::Point &point) const = 0;
};

}
}

#endif