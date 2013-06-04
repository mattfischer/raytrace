#include "Surface/Albedo/Base.hpp"

#include "Surface/Albedo/Solid.hpp"
#include "Surface/Albedo/Checker.hpp"

#include "Parse/AST.h"

namespace Surface {
namespace Albedo {

Base::Base()
{
}

Base::~Base()
{
}

const Base *Base::fromAst(AST *ast)
{
	AST *pigmentAst = ast->children[0];
	switch(pigmentAst->type)
	{
	case AstAlbedoSolid:
		return new Solid(pigmentAst->children[0]->data._vector);
	case AstAlbedoChecker:
		return new Checker(pigmentAst->children[0]->data._vector, pigmentAst->children[1]->data._vector);
	}

	return 0;
}

}
}