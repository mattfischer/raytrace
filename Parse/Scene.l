%option noyywrap

%{
    #include "Parse/AST.h"
    #include "Scene_yacc.h"
    #include <string.h>
    #include <stdlib.h>
    #include <io.h>
    #define isatty _isatty

    void parseVector(char *text, ASTVector *vector);
    char *parseString(char *text);
%}

FLOAT -?[[:digit:]]+(\.[[:digit:]]+)?
STRING \"[^"]*\"

%%
[[:blank:]]*
\n*

\<{FLOAT},{FLOAT},{FLOAT}\>    parseVector(Scenetext, &(Scenelval._vector)); return VECTOR;
{FLOAT} Scenelval._float = atof(Scenetext); return FLOAT;
{STRING} Scenelval._string = parseString(Scenetext); return STRING;

sphere    return SPHERE;
quad return QUAD;
model return MODEL;

transform return TRANSFORM;
rotate return ROTATE;
translate return TRANSLATE;
scale return SCALE;
uniform_scale return UNIFORM_SCALE;

surface return SURFACE;
radiance return RADIANCE;
brdf return BRDF;
albedo return ALBEDO;
color return COLOR;
texture return TEXTURE;
normal_map return NORMAL_MAP;

lambert return LAMBERT;
phong return PHONG;
oren_nayar return OREN_NAYAR;
torrance_sparrow return TORRANCE_SPARROW;
transmit return TRANSMIT;

camera return CAMERA;

point_light return POINT_LIGHT;
sky return SKY;

\{|\}        return *yytext;
<<EOF>>    return END;
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

char *parseString(char *text)
{
    char *c = (char*)malloc(strlen(text) - 1);
    strcpy(c, text + 1);
    c[strlen(c) - 1] = '\0';

    return c;
}
