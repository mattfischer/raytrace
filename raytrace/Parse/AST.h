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
	AstQuad,

	AstTransform,
	AstTranslate,
	AstRotate,
	AstScale,
	
	AstSurface,
	AstAlbedo,
	AstAlbedoSolid,
	AstAlbedoChecker,
	AstColor,
	AstRadiance,

	AstSurfaceDiffuse,
	AstBrdf,
	AstLambert,
	AstPhong,
	AstSpecular,
	AstSurfaceReflection,

	AstCamera,

	AstUnion,
	AstDifference,
	AstIntersection,

	AstConstant
} ASTType;

struct _ASTVector
{
	float x;
	float y;
	float z;
};
typedef struct _ASTVector ASTVector;

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