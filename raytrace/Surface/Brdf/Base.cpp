#include "Surface/Brdf/Base.hpp"

#include "Parse/AST.h"

#include "Surface/Brdf/Ambient.hpp"
#include "Surface/Brdf/Lambert.hpp"
#include "Surface/Brdf/Phong.hpp"
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
				brdfs.push_back(Ambient::fromAst(ast->children[i]));
				break;

			case AstLambert:
				brdfs.push_back(Lambert::fromAst(ast->children[i]));
				break;

			case AstPhong:
				brdfs.push_back(Phong::fromAst(ast->children[i]));
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