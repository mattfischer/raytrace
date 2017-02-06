#include "Object/Surface.hpp"

#include "Object/Scene.hpp"
#include "Object/Light.hpp"
#include "Object/Camera.hpp"
#include "Trace/Tracer.hpp"

#include "Object/Albedo/Base.hpp"
#include "Object/Brdf/Base.hpp"

#include "Parse/AST.h"

namespace Object {

	Surface::Surface(std::unique_ptr<Albedo::Base> &&albedo, std::unique_ptr<Brdf::Base> &&brdf)
	{
		mAlbedo = std::move(albedo);
		mBrdf = std::move(brdf);
	}

	Surface::~Surface()
	{
	}

	const Albedo::Base &Surface::albedo() const
	{
		return *mAlbedo;
	}

	const Brdf::Base &Surface::brdf() const
	{
		return *mBrdf;
	}

	std::unique_ptr<Surface> Surface::fromAst(AST *ast)
	{
		std::unique_ptr<Albedo::Base> albedo;
		std::unique_ptr<Brdf::Base> brdf;

		for (int i = 0; i < ast->numChildren; i++) {
			switch (ast->children[i]->type) {
			case AstAlbedo:
				albedo = Albedo::Base::fromAst(ast->children[i]);
				break;

			case AstBrdf:
				brdf = Brdf::Base::fromAst(ast->children[i]);
				break;
			}
		}

		return std::make_unique<Surface>(std::move(albedo), std::move(brdf));
	}
}