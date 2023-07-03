typedef struct {
    Color color;
} AlbedoSolid;

typedef enum {
    AlbedoTypeSolid,
    AlbedoTypeTexture
} AlbedoType;

typedef struct {
    AlbedoType type;
    union {
        AlbedoSolid solid;
    };
} Albedo;

Color AlbedoSolid_color(AlbedoSolid *solid)
{
    return solid->color;
}

Color Albedo_color(Albedo *albedo)
{
    switch(albedo->type) {
        case AlbedoTypeSolid:
            return AlbedoSolid_color(&albedo->solid);

        default:
            return (Color)(0, 0, 0);
    }
}