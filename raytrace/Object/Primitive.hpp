#ifndef OBJECT_PRIMITIVE_HPP
#define OBJECT_PRIMITIVE_HPP

#include "Math/Transformation.hpp"

#include "Object/Surface.hpp"
#include "Object/BoundingVolume.hpp"
#include "Object/Shape/Base.hpp"

namespace Object {

class Primitive
{
public:
	static std::unique_ptr<Primitive> fromAst(AST *ast);

	Primitive(std::unique_ptr<Shape::Base> shape, std::unique_ptr<Surface> surface, const Math::Transformation &transformation);

	const Shape::Base &shape() const;
	const Surface &surface() const;
	const Math::Transformation &transformation() const;

	float intersect(const Math::Ray &ray, Math::Normal &normal) const;
	const BoundingVolume &boundingVolume() const;
	bool sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const;

protected:
	std::unique_ptr<Shape::Base> mShape;
	Math::Transformation mTransformation;
	std::unique_ptr<Surface> mSurface;
	BoundingVolume mBoundingVolume;
};

}
#endif
