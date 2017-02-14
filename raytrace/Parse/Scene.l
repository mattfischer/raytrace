%option noyywrap

%{
	#include "Parse/AST.h"
	#include "Scene.tab.h"
	#include <string.h>
	#include <stdlib.h>
	
	void parseVector(char *text, ASTVector *vector);
%}

FLOAT -?[[:digit:]]+(\.[[:digit:]]+)?

%%
[[:blank:]]*
\n*

\<{FLOAT},{FLOAT},{FLOAT}\>	parseVector(Scenetext, &(Scenelval._vector)); return VECTOR;
{FLOAT} Scenelval._float = atof(Scenetext); return FLOAT;

sphere	return SPHERE;
plane return PLANE;
box return BOX;
cone return CONE;
cylinder return CYLINDER;

transform return TRANSFORM;
rotate return ROTATE;
translate return TRANSLATE;
scale return SCALE;

surface return SURFACE;
radiance return RADIANCE;
brdf return BRDF;
albedo return ALBEDO;
color return COLOR;
checker return CHECKER;

lambert return LAMBERT;
phong return PHONG;
specular return SPECULAR;

camera return CAMERA;

union return UNION;
difference return DIFFERENCE;
intersection return INTERSECTION;

\{|\}		return *yytext;
<<EOF>>	return END;
%%
void parseVector(char *text, ASTVector *vector)
{
	char *c;
	char b;
	
	text++;
	
	c = strchr(text, ',');
	b = *c;
	*c = '\0';
	vector->x = atof(text);
	*c = b;
	text = c + 1;
	
	c = strchr(text, ',');
	b = *c;
	*c = '\0';
	vector->y = atof(text);
	*c = b;
	text = c + 1;
	
	c = strchr(text, '>');
	b = *c;
	*c = '\0';
	vector->z = atof(text);
	*c = b;
}
	