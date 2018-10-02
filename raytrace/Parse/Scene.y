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
%token BOX
%token QUAD

%token TRANSFORM
%token TRANSLATE
%token ROTATE
%token SCALE

%token SURFACE
%token ALBEDO
%token COLOR
%token CHECKER

%token BRDF
%token LAMBERT
%token PHONG
%token SPECULAR
%token CAMERA
%token RADIANCE

%token END
%token <_vector> VECTOR
%token <_float> FLOAT

%type <_ast> scene object_list object primitive primitive_wrap primitive_modifiers primitive_modifier
%type <_ast> colordef
%type <_ast> transformdef transform_list transform_item
%type <_ast> surfacedef surface_list surface_item albedodef brdf_list brdf_item
%type <_ast> spheredef boxdef quaddef
%type <_ast> cameradef camera_modifiers camera_modifier
%start scene

%%
scene: object_list END
	{ tree = $1; }

object_list: object
	{ $$ = newAst(AstList, 1, $1); }
		   | object_list object
	{ $$ = addChild($1, $2); }
			  
object: primitive
	  | cameradef
	
primitive: primitive_wrap
	{ $$ = newAst(AstPrimitive, 1, $1); }
	
primitive_wrap: spheredef | boxdef | quaddef
	
spheredef: SPHERE '{' VECTOR FLOAT primitive_modifiers '}'
	{ $$ = newAst(AstSphere, 2, newAst(AstList, 2, newAst(AstConstant, 0), newAst(AstConstant, 0)), $5); 
	  $$->children[0]->children[0]->data._vector = $3;
	  $$->children[0]->children[1]->data._float = $4; }

boxdef: BOX '{' VECTOR VECTOR primitive_modifiers '}'
	{ $$ = newAst(AstBox, 2, newAst(AstList, 2, newAst(AstConstant, 0), newAst(AstConstant, 0)), $5);
	  $$->children[0]->children[0]->data._vector = $3;
	  $$->children[0]->children[1]->data._vector = $4; }

quaddef: QUAD '{' VECTOR VECTOR VECTOR primitive_modifiers '}'
    { $$ = newAst(AstQuad, 2, newAst(AstList, 3, newAst(AstConstant, 0), newAst(AstConstant, 0), newAst(AstConstant, 0)), $6);
	  $$->children[0]->children[0]->data._vector = $3;
	  $$->children[0]->children[1]->data._vector = $4;
	  $$->children[0]->children[2]->data._vector = $5;
	}

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

surface_item: ALBEDO '{' albedodef '}'
	{ $$ = newAst(AstAlbedo, 1, $3); }
			| BRDF '{' brdf_list '}'
	{ $$ = $3; }
			| RADIANCE VECTOR
	{ $$ = newAst(AstRadiance, 0); $$->data._vector = $2; }

brdf_list: brdf_item
	{ $$ = newAst(AstBrdf, 1, $1); }
			| brdf_list brdf_item
	{ $$ = addChild($1, $2); }

brdf_item: LAMBERT FLOAT
	{ $$ = newAst(AstLambert, 0); $$->data._float = $2; }
			| PHONG FLOAT FLOAT
	{ $$ = newAst(AstPhong, 2, newAst(AstConstant, 0), newAst(AstConstant, 0));
	  $$->children[0]->data._float = $2;
	  $$->children[1]->data._float = $3; }
			| SPECULAR FLOAT
	{ $$ = newAst(AstSpecular, 0); $$->data._float = $2; }

albedodef: colordef
	{ $$ = newAst(AstAlbedoSolid, 1, $1); }
			| CHECKER colordef colordef
	{ $$ = newAst(AstAlbedoChecker, 2, $2, $3); }

colordef: COLOR VECTOR
	{ $$ = newAst(AstColor, 0); $$->data._vector = $2; }	

cameradef: CAMERA '{' camera_modifiers '}'
	{ $$ = addChildren(newAst(AstCamera, 0), $3->numChildren, $3->children); }
	
camera_modifiers: camera_modifier
	{ $$ = newAst(AstList, 1, $1); }
				| camera_modifiers camera_modifier
	{ $$ = addChild($1, $2); }
	
camera_modifier: transformdef

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