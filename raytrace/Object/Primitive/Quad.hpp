#ifndef OBJECT_PRIMITIVE_QUAD_HPP
#define OBJECT_PRIMITIVE_QUAD_HPP

#include "Parse/AST.h"

#include "Object/Primitive/Sampleable.hpp"

namespace Object {
namespace Primitive {

class Quad : public Sampleable
{
public:
	static std::unique_ptr<Quad> fromAst(AST *ast);

	Quad(const Math::Point &position, const Math::Vector &side1, const Math::Vector &side2);

protected:
	virtual Intersection doIntersect(const Math::Ray &ray) const;
	virtual bool doInside(const Math::Point &point) const;
	virtual BoundingVolume doBoundingVolume() const;
	virtual void doSample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const;

private:
	Math::Point mPosition;
	Math::Vector mSide1;
	Math::Vector mSide2;
	Math::Normal mNormal;
};

}
}
#endif