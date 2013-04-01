#include "Object/Pigment/Base.hpp"

#include "Object/Pigment/Solid.hpp"
#include "Object/Pigment/Checker.hpp"

namespace Object {
namespace Pigment {

Base::Base()
{
}

Base::~Base()
{
}

Base *Base::fromAst(AST *ast)
{
	AST *pigmentAst = ast->children[0];
	switch(pigmentAst->type)
	{
	case AstPigmentColor:
		return new Solid(pigmentAst->children[0]->data._vector);
	case AstPigmentChecker:
		return new Checker(pigmentAst->children[0]->data._vector, pigmentAst->children[1]->data._vector);
	}

	return 0;
}

}
}