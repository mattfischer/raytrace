#include "Object/Shape/Base.hpp"

#include "Object/Shape/Sphere.hpp"
#include "Object/Shape/Quad.hpp"

#include <algorithm>

namespace Object {
namespace Shape {

std::unique_ptr<Base> Base::fromAst(AST *ast)
{
	std::unique_ptr<Base> shape;

	switch(ast->type)
	{
	case AstSphere:
		shape = Sphere::fromAst(ast);
		break;
	case AstQuad:
		shape = Quad::fromAst(ast);
		break;
	}

	return shape;
}

}
}