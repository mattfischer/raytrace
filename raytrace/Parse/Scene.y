%{
	#include "Parse/AST.h"
	#include <stdarg.h>
	#include <stdio.h>
		
	AST *newAst(ASTType type, int numChildren, ...);
	AST *addChild(AST *ast, AST *child);
	AST *addChildren(AST *ast, int numChildren, AST *children[]);
	
	static AST *tree;
%}

%union
{
	AST *_ast;
	ASTVector _vector;
	float _float;
};

%token SPHERE
%token PLANE
%token CONE
%token CYLINDER
%token BOX

%token TRANSFORM
%token TRANSLATE
%token ROTATE
%token SCALE

%token SURFACE
%token ALBEDO
%token COLOR
%token CHECKER

%token DIFFUSE
%token BRDF
%token AMBIENT
%token LAMBERT
%token PHONG
%token SPECULAR
%token REFLECTION
%token CAMERA
%token LIGHT


%token UNION
%token DIFFERENCE
%token INTERSECTION

%token END
%token <_vector> VECTOR
%token <_float> FLOAT

%type <_ast> scene object_list object primitive primitive_wrap primitive_modifiers primitive_modifier
%type <_ast> colordef
%type <_ast> transformdef transform_list transform_item
%type <_ast> surfacedef surface_list surface_item diffusedef albedodef diffuse_list diffuse_item reflectiondef brdf_list brdf_item
%type <_ast> spheredef planedef boxdef conedef cylinderdef
%type <_ast> lightdef light_modifiers light_modifier
%type <_ast> cameradef camera_modifiers camera_modifier csgdef
%start scene

%%
scene: object_list END
	{ tree = $1; }

object_list: object
	{ $$ = newAst(AstList, 1, $1); }
		   | object_list object
	{ $$ = addChild($1, $2); }
			  
object: primitive
	  | lightdef
	  | cameradef
	
primitive: primitive_wrap
	{ $$ = newAst(AstPrimitive, 1, $1); }
	
primitive_wrap: spheredef | planedef | boxdef | conedef | cylinderdef | csgdef
	
spheredef: SPHERE '{' primitive_modifiers '}'
	{ $$ = addChildren(newAst(AstSphere, 0), $3->numChildren, $3->children); }

planedef: PLANE '{' primitive_modifiers '}'
	{ $$ = addChildren(newAst(AstPlane, 0), $3->numChildren, $3->children); }

boxdef: BOX '{' primitive_modifiers '}'
	{ $$ = addChildren(newAst(AstBox, 0), $3->numChildren, $3->children); }

conedef: CONE '{' primitive_modifiers '}'
	{ $$ = addChildren(newAst(AstCone, 0), $3->numChildren, $3->children); }

cylinderdef: CYLINDER '{' primitive_modifiers '}'
	{ $$ = addChildren(newAst(AstCylinder, 0), $3->numChildren, $3->children); }

primitive_modifiers: primitive_modifier
	{ $$ = newAst(AstList, 1, $1); }
				   | primitive_modifiers primitive_modifier
	{ $$ = addChild($1, $2); }

primitive_modifier: transformdef | surfacedef
	
transformdef: TRANSFORM '{' transform_list '}'
	{ $$ = $3; }
	
transform_list: transform_item
	{ $$ = newAst(AstTransform, 1, $1); }
			|  transform_list transform_item
	{ $$ = addChild($1, $2); }
				
transform_item: TRANSLATE VECTOR
	{ $$ = newAst(AstTranslate, 0); $$->data._vector = $2; }
			 | ROTATE VECTOR
	{ $$ = newAst(AstRotate, 0); $$->data._vector = $2; }
			 | SCALE VECTOR
	{ $$ = newAst(AstScale, 0); $$->data._vector = $2; }

surfacedef: SURFACE '{' surface_list '}'
	{ $$ = $3; }
	
surface_list: surface_item
	{ $$ = newAst(AstSurface, 1, $1); }
		   | surface_list surface_item
	{ $$ = addChild($1, $2); }

surface_item: diffusedef | reflectiondef

diffusedef: DIFFUSE '{' diffuse_list '}'
	{ $$ = $3; }

diffuse_list: diffuse_item
	{ $$ = newAst(AstSurfaceDiffuse, 1, $1); }
			| diffuse_list diffuse_item
	{ $$ = addChild($1, $2); }

diffuse_item: ALBEDO '{' albedodef '}'
	{ $$ = newAst(AstAlbedo, 1, $3); }
			| BRDF '{' brdf_list '}'
	{ $$ = $3; }

brdf_list: brdf_item
	{ $$ = newAst(AstBrdf, 1, $1); }
			| brdf_list brdf_item
	{ $$ = addChild($1, $2); }

brdf_item: AMBIENT FLOAT
	{ $$ = newAst(AstAmbient, 0); $$->data._float = $2; }
			| LAMBERT FLOAT
	{ $$ = newAst(AstLambert, 0); $$->data._float = $2; }
			| PHONG FLOAT FLOAT
	{ $$ = newAst(AstPhong, 1, newAst(AstFloat, 0)); $$->data._float = $2; $$->children[0]->data._float = $3; }
			| SPECULAR FLOAT
	{ $$ = newAst(AstSpecular, 0); $$->data._float = $2; }

albedodef: colordef
	{ $$ = newAst(AstAlbedoSolid, 1, $1); }
			| CHECKER colordef colordef
	{ $$ = newAst(AstAlbedoChecker, 2, $2, $3); }

reflectiondef: REFLECTION
	{ $$ = newAst(AstSurfaceReflection, 0); }

colordef: COLOR VECTOR
	{ $$ = newAst(AstColor, 0); $$->data._vector = $2; }	
	
lightdef: LIGHT '{' colordef light_modifiers '}'
	{ $$ = addChildren(newAst(AstLight, 1, $3), $4->numChildren, $4->children); }
	
light_modifiers: light_modifier
	{ $$ = newAst(AstList, 1, $1); }
			   | light_modifiers light_modifier
	{ $$ = addChild($1, $2); }

light_modifier: transformdef

cameradef: CAMERA '{' camera_modifiers '}'
	{ $$ = addChildren(newAst(AstCamera, 0), $3->numChildren, $3->children); }
	
camera_modifiers: camera_modifier
	{ $$ = newAst(AstList, 1, $1); }
				| camera_modifiers camera_modifier
	{ $$ = addChild($1, $2); }
	
camera_modifier: transformdef

csgdef: UNION '{' primitive primitive primitive_modifiers '}'
	{ $$ = addChildren(newAst(AstUnion, 2, $3, $4), $5->numChildren, $5->children); }
	  | DIFFERENCE '{' primitive primitive primitive_modifiers '}'
	{ $$ = addChildren(newAst(AstDifference, 2, $3, $4), $5->numChildren, $5->children); }
	  | INTERSECTION '{' primitive primitive primitive_modifiers '}'
	{ $$ = addChildren(newAst(AstIntersection, 2, $3, $4), $5->numChildren, $5->children); }

%%

extern FILE *Scenein;

AST *parseScene(const char *filename)
{
	fopen_s(&Scenein, filename, "r");
	Sceneparse();
	
	return tree;
}

int Sceneerror(char const *s)
{
	return 0;
}

AST *newAst(ASTType type, int numChildren, ...)
{
	va_list va;
	int i;
	
	AST *ast = malloc(sizeof(AST));
	
	ast->type = type;
	ast->numChildren = numChildren;
	ast->children = (AST**)malloc(numChildren * sizeof(AST*));
	
	va_start(va, numChildren);
	for(i=0; i<numChildren; i++)
	{
		ast->children[i] = va_arg(va, AST*);
	}
	
	va_end(va);
	
	return ast;
}

AST *addChild(AST *ast, AST *child)
{
	ast->numChildren++;
	ast->children = (AST**)realloc(ast->children, ast->numChildren * sizeof(AST*));
	
	ast->children[ast->numChildren - 1] = child;
	
	return ast;
}

AST *addChildren(AST *ast, int numChildren, AST *children[])
{
	int i;
	
	ast->children = (AST**)realloc(ast->children, (ast->numChildren + numChildren) * sizeof(AST*));
	
	for(i=0; i<numChildren; i++)
	{
		ast->children[ast->numChildren + i] = children[i];
	}
	
	ast->numChildren += numChildren;
	
	return ast;
}