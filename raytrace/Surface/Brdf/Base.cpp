#include "Surface/Brdf/Base.hpp"

#include "Parse/AST.h"

#include "Surface/Brdf/Ambient.hpp"
#include "Surface/Brdf/Lambert.hpp"
#include "Surface/Brdf/Specular.hpp"
#include "Surface/Brdf/Composite.hpp"

#include <vector>

namespace Surface {
namespace Brdf {

const Base *Base::fromAst(AST *ast)
{
	std::vector<const Base*> brdfs;

	for(int i=0; i<ast->numChildren; i++) {
		switch(ast->children[i]->type) {
			case AstAmbient:
				brdfs.push_back(new Ambient(ast->children[i]->data._float));
				break;

			case AstLambert:
				brdfs.push_back(new Lambert(ast->children[i]->data._float));
				break;

			case AstSpecular:
				brdfs.push_back(new Specular(ast->children[i]->data._float, ast->children[i]->children[0]->data._float));
				break;
		}
	}

	if(brdfs.size() == 1) {
		return brdfs[0];
	} else {
		return new Composite(brdfs);
	}
}

}
}