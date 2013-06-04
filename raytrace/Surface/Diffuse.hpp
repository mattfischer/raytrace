#ifndef SURFACE_DIFFUSE_HPP
#define SURFACE_DIFFUSE_HPP

#include "Surface/Base.hpp"

#include "Surface/Albedo/Forwards.hpp"
#include "Surface/Brdf/Forwards.hpp"

namespace Surface {

class Diffuse : public Base
{
public:
	Diffuse(const Albedo::Base *albedo, const Brdf::Base *brdf);
	virtual ~Diffuse();

	static Diffuse *fromAst(AST *ast);

	virtual Object::Color color(const Trace::Intersection &intersection, Trace::Tracer &tracer) const;

private:
	const Albedo::Base *mAlbedo;
	const Brdf::Base *mBrdf;
};

}

#endif