#include "buildscene.h"

#include "ast.h"

#include <stdio.h>

extern "C"
{
	AST *parseScene(const char *filename);
}

Object::Scene *buildScene()
{
	AST *ast = parseScene("scene.txt");

	return Object::Scene::fromAST(ast);
}