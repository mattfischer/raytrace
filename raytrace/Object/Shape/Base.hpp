#ifndef OBJECT_SHAPE_BASE_HPP
#define OBJECT_SHAPE_BASE_HPP

#include "Math/Ray.hpp"
#include "Math/Normal.hpp"
#include "Math/Transformation.hpp"

#include "Object/BoundingVolume.hpp"

#include <vector>
#include <memory>

namespace Object {
namespace Shape {

class Base
{
public:
	static std::unique_ptr<Base> fromAst(AST *ast);

	virtual float intersect(const Math::Ray &ray, Math::Normal &normal) const = 0;
	virtual BoundingVolume boundingVolume(const Math::Transformation &transformation) const = 0;
	virtual bool sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const = 0;
};

}
}

#endif