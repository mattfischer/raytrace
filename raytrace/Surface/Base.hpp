#ifndef SURFACE_BASE_HPP
#define SURFACE_BASE_HPP

#include "Object/Forwards.hpp"
#include "Trace/Forwards.hpp"

#include "Object/Color.hpp"

namespace Surface {

class Base
{
public:
	Base();
	virtual ~Base();

	static Base *fromAst(AST *ast);

	virtual Object::Color color(const Trace::Intersection &intersection, Trace::Tracer &tracer) const = 0;
};

}

#endif