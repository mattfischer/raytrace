typedef struct {
    int width;
    int height;
    int numChannels;
    float *values;
} MipMap;

typedef struct {
    int numMipMaps;
    MipMap *mipMaps;
} Texture;

typedef struct {
    Color color;
} AlbedoSolid;

typedef struct {
    Texture texture;
} AlbedoTexture;

typedef enum {
    AlbedoTypeSolid,
    AlbedoTypeTexture
} AlbedoType;

typedef struct {
    AlbedoType type;
    union {
        AlbedoSolid solid;
        AlbedoTexture texture;
    };
} Albedo;

Color AlbedoSolid_color(AlbedoSolid *solid, Point2D surfacePoint, Bivector2D *surfaceProjection)
{
    return solid->color;
}

float3 MipMap_sample(MipMap *mipMap, Point2D samplePoint)
{
    float fx = samplePoint.x * mipMap->width;
    float fy = samplePoint.y * mipMap->height;
    int x = (int)floor(fx);
    int y = (int)floor(fy);
    int x1 = (x == mipMap->width - 1) ? 0 : x + 1;
    int y1 = (y == mipMap->height - 1) ? 0 : y + 1;
    float dx = fx - x;
    float dy = fy - y;

    float3 p0 = (float3)(mipMap->values[(y * mipMap->width + x) * mipMap->numChannels + 0],
                        mipMap->values[(y * mipMap->width + x) * mipMap->numChannels + 1],
                        mipMap->values[(y * mipMap->width + x) * mipMap->numChannels + 2]);

    float3 p1 = (float3)(mipMap->values[(y * mipMap->width + x1) * mipMap->numChannels + 0],
                        mipMap->values[(y * mipMap->width + x1) * mipMap->numChannels + 1],
                        mipMap->values[(y * mipMap->width + x1) * mipMap->numChannels + 2]);

    float3 p2 = (float3)(mipMap->values[(y1 * mipMap->width + x) * mipMap->numChannels + 0],
                        mipMap->values[(y1 * mipMap->width + x) * mipMap->numChannels + 1],
                        mipMap->values[(y1 * mipMap->width + x) * mipMap->numChannels + 2]);

    float3 p3 = (float3)(mipMap->values[(y1 * mipMap->width + x1) * mipMap->numChannels + 0],
                        mipMap->values[(y1 * mipMap->width + x1) * mipMap->numChannels + 1],
                        mipMap->values[(y1 * mipMap->width + x1) * mipMap->numChannels + 2]);

    float3 v = (1 - dx) * (1 - dy) * p0 + dx * (1 - dy) * p1 + (1 - dx) * dy * p2 + dx * dy * p3;
    
    return v;
}

float3 Texture_sample(Texture *texture, Point2D samplePoint, Bivector2D *sampleProjection)
{
    float projectionSize = min(length(sampleProjection->u), length(sampleProjection->v));

    float level = texture->numMipMaps - 1 + log2(projectionSize);
    float lf = max(0.0f, min(level, (float)texture->numMipMaps - 1));

    int l = (int)floor(lf);
    MipMap *level0 = &texture->mipMaps[l];
    MipMap *level1 = &texture->mipMaps[min(l + 1, (int)texture->numMipMaps - 1)];
    float dl = lf - l;

    return MipMap_sample(level0, samplePoint) * (1 - dl) + MipMap_sample(level1, samplePoint) * dl;
}

Color AlbedoTexture_color(AlbedoTexture *texture, Point2D surfacePoint, Bivector2D *surfaceProjection)
{
    return Texture_sample(&texture->texture, surfacePoint, surfaceProjection);
}

Color Albedo_color(Albedo *albedo, Point2D surfacePoint, Bivector2D *surfaceProjection)
{
    switch(albedo->type) {
        case AlbedoTypeSolid:
            return AlbedoSolid_color(&albedo->solid, surfacePoint, surfaceProjection);

        case AlbedoTypeTexture:
            return AlbedoTexture_color(&albedo->texture, surfacePoint, surfaceProjection);

        default:
            return (Color)(0, 0, 0);
    }
}