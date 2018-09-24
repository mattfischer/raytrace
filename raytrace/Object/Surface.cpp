#include "Object/Surface.hpp"

#include "Object/Albedo/Base.hpp"
#include "Object/Brdf/Base.hpp"

#include "Parse/AST.h"

namespace Object {

Surface::Surface(std::unique_ptr<Albedo::Base> &&albedo, std::unique_ptr<Brdf::Base> &&brdf, const Object::Radiance &radiance)
{
	mAlbedo = std::move(albedo);
	mBrdf = std::move(brdf);
	mRadiance = radiance;
}

const Albedo::Base &Surface::albedo() const
{
	return *mAlbedo;
}

const Brdf::Base &Surface::brdf() const
{
	return *mBrdf;
}

const Object::Radiance &Surface::radiance() const
{
	return mRadiance;
}

std::unique_ptr<Surface> Surface::fromAst(AST *ast)
{
	std::unique_ptr<Albedo::Base> albedo;
	std::unique_ptr<Brdf::Base> brdf;
	Object::Radiance radiance;

	for (int i = 0; i < ast->numChildren; i++) {
		switch (ast->children[i]->type) {
		case AstAlbedo:
			albedo = Albedo::Base::fromAst(ast->children[i]);
			break;

		case AstBrdf:
			brdf = Brdf::Base::fromAst(ast->children[i]);
			break;

		case AstRadiance:
			radiance = ast->children[i]->data._vector;
			break;
		}
	}

	return std::make_unique<Surface>(std::move(albedo), std::move(brdf), radiance);
}

}