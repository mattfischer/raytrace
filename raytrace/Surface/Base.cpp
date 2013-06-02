#include "Surface/Base.hpp"

#include "Surface/Diffuse.hpp"
#include "Surface/Reflection.hpp"
#include "Surface/Composite.hpp"

#include "Parse/AST.h"

namespace Surface {

Base::Base()
{
}

Base::~Base()
{
}

Base *Base::fromAst(AST *ast)
{
	Base **surfaces = new Base*[ast->numChildren];

	for(int i=0; i<ast->numChildren; i++) {
		switch(ast->children[i]->type) {
			case AstSurfaceDiffuse:
				surfaces[i] = Diffuse::fromAst(ast->children[i]);
				break;

			case AstSurfaceReflection:
				surfaces[i] = new Reflection();
				break;
		}
	}

	if(ast->numChildren == 1) {
		Base *surface = surfaces[0];
		delete[] surfaces;
		return surface;
	} else {
		return new Composite(surfaces, ast->numChildren);
	}
}

}