#include "Object/Surface.hpp"

#include "Object/Scene.hpp"
#include "Object/Light.hpp"
#include "Object/Camera.hpp"
#include "Trace/Tracer.hpp"

#include "Object/Albedo/Base.hpp"
#include "Object/Brdf/Base.hpp"

#include "Parse/AST.h"

namespace Object {

	Surface::Surface(const Albedo::Base *albedo, const Brdf::Base *brdf)
	{
		mAlbedo = albedo;
		mBrdf = brdf;
	}

	Surface::~Surface()
	{
		if (mAlbedo) {
			delete mAlbedo;
		}

		if (mBrdf) {
			delete mBrdf;
		}
	}

	const Albedo::Base *Surface::albedo() const
	{
		return mAlbedo;
	}

	const Brdf::Base *Surface::brdf() const
	{
		return mBrdf;
	}

	Surface *Surface::fromAst(AST *ast)
	{
		const Albedo::Base *albedo;
		const Brdf::Base *brdf;

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

		return new Surface(albedo, brdf);
	}
}