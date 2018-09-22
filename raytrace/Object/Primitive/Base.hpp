#ifndef OBJECT_PRIMITIVE_BASE_HPP
#define OBJECT_PRIMITIVE_BASE_HPP

#include "Object/Forwards.hpp"
#include "Trace/Forwards.hpp"
#include "Math/Forwards.hpp"

#include "Object/Base.hpp"
#include "Object/Surface.hpp"
#include "Trace/Intersection.hpp"
#include "Object/Primitive/BoundingVolume.hpp"

#include <vector>
#include <memory>

namespace Object {
namespace Primitive {

class Base : public Object::Base
{
public:
	static std::unique_ptr<Base> fromAst(AST *ast);

	const Surface &surface() const;
	void setSurface(std::unique_ptr<Surface> &&surface);

	void intersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const;
	bool inside(const Math::Point &point) const;
	void sample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const;

	const BoundingVolume &boundingVolume() const;

	virtual bool canSample() const;

protected:
	virtual void doTransform();

	static void parseAstCommon(Base &base, AST *ast);

	std::unique_ptr<Surface> mSurface;
	BoundingVolume mBoundingVolume;

	virtual void doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const = 0;
	virtual bool doInside(const Math::Point &point) const = 0;
	virtual BoundingVolume doBoundingVolume(const std::vector<Math::Vector> &vectors) const = 0;
	virtual void doSample(float u, float v, Math::Point &point, Math::Vector &du, Math::Vector &dv, Math::Normal &normal) const;
};

}
}

#endif