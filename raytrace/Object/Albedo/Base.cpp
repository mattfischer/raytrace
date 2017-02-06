#include "Object/Albedo/Base.hpp"

#include "Object/Albedo/Solid.hpp"
#include "Object/Albedo/Checker.hpp"

#include "Parse/AST.h"

namespace Object {
namespace Albedo {

std::unique_ptr<Base> Base::fromAst(AST *ast)
{
	AST *albedoAst = ast->children[0];
	switch(albedoAst->type)
	{
	case AstAlbedoSolid:
		return Solid::fromAst(albedoAst);
	case AstAlbedoChecker:
		return Checker::fromAst(albedoAst);
	}

	return 0;
}

}
}