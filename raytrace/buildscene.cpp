#include "buildscene.h"

#include "ast.h"

#include <stdio.h>

extern "C"
{
	AST *parseScene(const char *filename);
}

Scene *buildScene(int screenX, int screenY)
{
	Scene *scene = new Scene((double)screenY / (double)screenX);

	AST *tree = parseScene("scene.txt");

	for(int i=0; i<tree->numChildren; i++)
	{
		AST *ast = tree->children[i];

		switch(ast->type)
		{
		case AstPrimitive:
			scene->addPrimitive(Primitive::fromAst(ast));
			break;
		case AstLight:
			scene->addLight(Light::fromAst(ast));
			break;
		case AstCamera:
			scene->setCamera(Camera::fromAst(ast));
			break;
		}
	}

	return scene;
}