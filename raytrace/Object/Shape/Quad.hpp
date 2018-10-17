#ifndef OBJECT_SHAPE_QUAD_HPP
#define OBJECT_SHAPE_QUAD_HPP

#include "Parse/AST.h"

#include "Object/Shape/Base.hpp"

namespace Object {
namespace Shape {

class Quad : public Base
{
public:
	static std::unique_ptr<Quad> fromAst(AST *ast);

	Quad(const Math::Point &position, const Math::Vector &side1, const Math::Vector &side2);

	virtual bool intersect(const Math::Ray &ray, float &distance, Math::Normal &normal) const;
	virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const;
	virtual bool sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const;

private:
	Math::Point mPosition;
	Math::Vector mSide1;
	Math::Vector mSide2;
	Math::Normal mNormal;
};

}
}
#endif