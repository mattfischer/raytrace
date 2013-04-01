#include "buildscene.h"

#include "ast.h"

#include <stdio.h>

extern "C"
{
	AST *parseScene(const char *filename);
}

Scene *buildScene()
{
	Scene *scene = new Scene;

	AST *tree = parseScene("scene.txt");

	for(int i=0; i<tree->numChildren; i++)
	{
		AST *ast = tree->children[i];

		switch(ast->type)
		{
		case AstPrimitive:
			scene->addPrimitive(Object::Primitive::fromAst(ast));
			break;
		case AstLight:
			scene->addLight(Object::Light::fromAst(ast));
			break;
		case AstCamera:
			scene->setCamera(Object::Camera::fromAst(ast));
			break;
		}
	}

	return scene;
}