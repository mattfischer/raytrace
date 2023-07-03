typedef struct {
    Color color;
} SolidAlbedo;

typedef enum {
    AlbedoTypeSolid,
    AlbedoTypeTexture
} AlbedoType;

typedef struct {
    AlbedoType type;
    union {
        SolidAlbedo solid;
    };
} Albedo;
