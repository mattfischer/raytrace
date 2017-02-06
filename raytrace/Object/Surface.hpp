#ifndef OBJECT_SURFACE_HPP
#define OBJECT_SURFACE_HPP

#include "Object/Albedo/Base.hpp"
#include "Object/Brdf/Base.hpp"

#include "Object/Color.hpp"

#include <memory>

namespace Object {

class Surface
{
public:
	Surface(std::unique_ptr<Albedo::Base> &&albedo, std::unique_ptr<Brdf::Base> &&brdf);

	const Albedo::Base &albedo() const;
	const Brdf::Base &brdf() const;

	static std::unique_ptr<Surface> fromAst(AST *ast);

private:
	std::unique_ptr<Albedo::Base> mAlbedo;
	std::unique_ptr<Brdf::Base> mBrdf;
};

}
#endif