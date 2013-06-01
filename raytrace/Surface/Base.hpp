#ifndef SURFACE_BASE_HPP
#define SURFACE_BASE_HPP

#include "Object/Color.hpp"

namespace Object {
	class Scene;
}

namespace Trace {
	class Ray;
	class Intersection;
	class Tracer;
}

namespace Surface {

class Base
{
public:
	Base();
	virtual ~Base();

	static Base *fromAst(AST *ast);

	virtual Object::Color color(const Trace::Intersection &intersection, const Trace::Tracer &tracer) const = 0;
};

}

#endif