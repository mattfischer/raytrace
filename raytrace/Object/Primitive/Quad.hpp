#ifndef OBJECT_PRIMITIVE_QUAD_HPP
#define OBJECT_PRIMITIVE_QUAD_HPP

#include "Parse/Forwards.hpp"

#include "Object/Primitive/Base.hpp"

namespace Object {
namespace Primitive {

class Quad : public Base
{
public:
	static std::unique_ptr<Quad> fromAst(AST *ast);

	virtual bool canSample() const;

protected:
	virtual Intersection doIntersect(const Math::Ray &ray) const;
	virtual bool doInside(const Math::Point &point) const;
	virtual BoundingVolume doBoundingVolume(const std::vector<Math::Vector> &vectors) const;
	virtual void doSample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const;

};

}
}
#endif