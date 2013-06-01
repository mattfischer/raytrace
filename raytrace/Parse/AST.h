#ifndef PARSE_AST_H
#define PARSE_AST_H

typedef enum
{
	AstList,

	AstPrimitive,
	AstSphere,
	AstPlane,
	AstBox,
	AstCone,
	AstCylinder,

	AstTransform,
	AstTranslate,
	AstRotate,
	AstScale,
	
	AstTexture,
	AstPigment,
	AstPigmentColor,
	AstPigmentChecker,
	AstColor,

	AstFinish,
	AstAmbient,
	AstDiffuse,
	AstSpecular,
	AstSpecularPower,
	AstReflection,

	AstLight,

	AstCamera,

	AstUnion,
	AstDifference,
	AstIntersection
} ASTType;

typedef struct
{
	float x;
	float y;
	float z;
} ASTVector;

struct _AST;
typedef struct _AST AST;

struct _AST
{
	ASTType type;
	int numChildren;
	AST **children;
	union
	{
		ASTVector _vector;
		float _float;
	} data;
};

#endif