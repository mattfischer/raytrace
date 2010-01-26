#ifndef AST_H
#define AST_H

typedef enum
{
	AstPrimitiveList,
	AstSphere,
	AstPlane,
	AstBox,
	AstCone,
	AstCylinder,

	AstPrimitiveModifiers,

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
	AstReflection
} ASTType;

typedef struct
{
	double x;
	double y;
	double z;
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
		double _double;
	} data;
};

#endif