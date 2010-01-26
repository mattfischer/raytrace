#include "buildscene.h"

#include "sphere.h"
#include "box.h"
#include "plane.h"
#include "cone.h"
#include "disk.h"
#include "cylinder.h"
#include "csg.h"

#include "transformation.h"

#include "texture.h"

#include "ast.h"

#include <stdio.h>

extern "C"
{
	AST *parseScene(const char *filename);
}

Primitive *createPrimitive(AST *ast)
{
	int i;
	Primitive *primitive = 0;

	switch(ast->type)
	{
	case AstSphere:
		primitive = Sphere::fromAst(ast);
		break;
	case AstPlane:
		primitive = Plane::fromAst(ast);
		break;
	case AstBox:
		primitive = Box::fromAst(ast);
		break;
	case AstCone:
		primitive = Cone::fromAst(ast);
		break;
	case AstCylinder:
		primitive = Cylinder::fromAst(ast);
		break;
	}

	for(i=0; i<ast->numChildren; i++)
	{
		switch(ast->children[i]->type)
		{
		case AstTransform:
			primitive->transform(Transformation::fromAst(ast->children[i]));
			break;
		case AstTexture:
			primitive->setTexture(Texture::fromAst(ast->children[i]));
			break;
		}
	}

	return primitive;
}

Scene *buildScene(int screenX, int screenY)
{
	Scene *scene = new Scene;

	Camera *camera = new Camera(45, (double)screenY / (double)screenX);
	camera->transform(Transformation::translate(3, 3, -10));
	camera->transform(Transformation::rotate(0, 20, 0));
	camera->transform(Transformation::rotate(20, 0, 0));
	scene->setCamera(camera);

	Light *light = new Light(Color(1, 1, 1));
	light->transform(Transformation::translate(3, 2, -3));
	scene->addLight(light);

	AST *ast = parseScene("scene.txt");

	for(int i=0; i<ast->numChildren; i++)
	{
		AST *primAST = ast->children[i];
		Primitive *primitive = createPrimitive(ast->children[i]);

		scene->addPrimitive(primitive);
	}

	return scene;
}