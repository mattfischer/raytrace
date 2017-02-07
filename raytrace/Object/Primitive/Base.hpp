#ifndef OBJECT_PRIMITIVE_BASE_HPP
#define OBJECT_PRIMITIVE_BASE_HPP

#include "Object/Forwards.hpp"
#include "Trace/Forwards.hpp"
#include "Math/Forwards.hpp"

#include "Object/Base.hpp"
#include "Object/Surface.hpp"
#include "Trace/Intersection.hpp"

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

protected:
	static void parseAstCommon(Base &base, AST *ast);

	std::unique_ptr<Surface> mSurface;

	virtual void doIntersect(const Trace::Ray &ray, Trace::IntersectionVector &intersections) const = 0;
	virtual bool doInside(const Math::Point &point) const = 0;
};

}
}

#endif