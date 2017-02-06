#ifndef OBJECT_SURFACE_HPP
#define OBJECT_SURFACE_HPP

#include "Object/Albedo/Forwards.hpp"
#include "Object/Brdf/Forwards.hpp"
#include "Trace/Forwards.hpp"

#include "Object/Color.hpp"

namespace Object {

class Surface
{
public:
Surface(const Albedo::Base *albedo, const Brdf::Base *brdf);
virtual ~Surface();

static Surface *fromAst(AST *ast);

virtual Object::Color color(const Trace::Intersection &intersection, Trace::Tracer &tracer) const;

private:
const Albedo::Base *mAlbedo;
const Brdf::Base *mBrdf;
};

}
#endif