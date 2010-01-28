%option noyywrap

%{
	#include "ast.h"
	#include "scene.tab.h"
	#include <string.h>
	#include <stdlib.h>
	
	void parseVector(char *text, ASTVector *vector);
%}

FLOAT -?[[:digit:]]+(\.[[:digit:]]+)?

%%
[[:blank:]]*
\n*

\<{FLOAT},{FLOAT},{FLOAT}\>	parseVector(scenetext, &(scenelval._vector)); return VECTOR;
{FLOAT} scenelval._double = atof(scenetext); return DOUBLE;

sphere	return SPHERE;
plane return PLANE;
box return BOX;
cone return CONE;
cylinder return CYLINDER;

transform return TRANSFORM;
rotate return ROTATE;
translate return TRANSLATE;
scale return SCALE;

texture return TEXTURE;
pigment return PIGMENT;
color return COLOR;
checker return CHECKER;

finish return FINISH;
ambient return AMBIENT;
diffuse return DIFFUSE;
specular return SPECULAR;
specular_power return SPECULAR_POWER;
reflection return REFLECTION;

light return LIGHT;

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
	