#ifndef PARSE_AST_H
#define PARSE_AST_H

typedef enum
{
	AstList,

	AstPrimitive,
	AstSphere,
	AstQuad,
	AstModel,

	AstTransform,
	AstTranslate,
	AstRotate,
	AstScale,
	AstUniformScale,

	AstSurface,
	AstAlbedo,
	AstAlbedoSolid,
	AstAlbedoTexture,
	AstColor,
	AstRadiance,
	AstNormalMap,

	AstBrdf,
	AstBrdfLambert,
	AstBrdfPhong,
	AstBrdfOrenNayar,
	AstBrdfTorranceSparrow,
	AstBrdfTransmit,

	AstCamera,

	AstPointLight,

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
		char *_string;
	} data;
};

#endif