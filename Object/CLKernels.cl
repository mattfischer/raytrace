typedef struct {
    Radiance radiance;
    Albedo albedo;
    int numBrdfs;
    Brdf *brdfs;
} Surface;

typedef struct {
    Shape shape;
    Surface surface;
} Primitive;

typedef struct {
    Point position;
    Radiance radiance;
} PointLight;

typedef struct {
    Point position;
    Vector direction;
    Bivector imagePlane;
    float imageSize;
    float focalLength;
    float apertureSize;
} Camera;

typedef struct {
    int numPrimitives;
    Primitive *primitives;
    int numAreaLights;
    Primitive **areaLights;
    int numPointLights;
    PointLight *pointLights;
    Radiance skyRadiance;
    Camera camera;
} Scene;

typedef struct {
    ShapeIntersection shapeIntersection;
    Primitive *primitive;
    Beam *beam;
    Point point;
} Intersection;

Normal facingNormal(Intersection *isect)
{
    if(dot(isect->shapeIntersection.normal, isect->beam->ray.direction) > 0) {
        return -isect->shapeIntersection.normal;
    } else {
        return isect->shapeIntersection.normal;
    }
}

float Surface_pdf(Intersection *isect, Vector dirIn)
{
    Surface *surf = &isect->primitive->surface;
    Vector dirOut = -isect->beam->ray.direction;
    Normal nrmFacing = facingNormal(isect);

    float totalPdf = 0;
    for(int i=0; i<surf->numBrdfs; i++) {
        totalPdf += Brdf_pdf(&surf->brdfs[i], dirIn, nrmFacing, dirOut);
    }
    totalPdf /= surf->numBrdfs;

    return totalPdf;
}

Color Surface_reflected(Intersection *isect, Vector dirIn)
{
    Surface *surf = &isect->primitive->surface;
    Vector dirOut = -isect->beam->ray.direction;
    Normal nrmFacing = facingNormal(isect);
    Color albedo = Albedo_color(&surf->albedo);

    Color col = (Color)(0, 0, 0);
    Color colTransmit = (Color)(1, 1, 1);
    for(int i=0; i<surf->numBrdfs; i++) {
        col = col + colTransmit * Brdf_reflected(&surf->brdfs[i], dirIn, nrmFacing, dirOut, albedo);
        colTransmit = colTransmit * Brdf_transmitted(&surf->brdfs[i], dirIn, nrmFacing, albedo);
    }

    return col;
}

Color Surface_sample(Intersection *isect, float3 random, Vector *dirIn, float *pdf, bool *pdfDelta)
{
    Surface *surf = &isect->primitive->surface;
    Vector dirOut = -isect->beam->ray.direction;
    Normal nrmFacing = facingNormal(isect);

    int idx = 0;
    if(surf->numBrdfs > 1) {
        idx = (int)floor(surf->numBrdfs * random.x);
    }

    *dirIn = Brdf_sample(&surf->brdfs[idx], random.yz, nrmFacing, dirOut);
    *pdf = Surface_pdf(isect, *dirIn);
    *pdfDelta = false;
    return Surface_reflected(isect, *dirIn);
}

void Scene_intersect(Scene *scene, Beam *beam, Intersection *isect)
{
    isect->shapeIntersection.distance = MAXFLOAT;
    isect->primitive = 0;
    isect->beam = beam;

    for(int i=0; i<scene->numPrimitives; i++) {
        if(Shape_intersect(&beam->ray, &scene->primitives[i].shape, &isect->shapeIntersection)) {
            isect->primitive = &scene->primitives[i];
        }
    }

    if(isect->primitive != 0) {
        isect->point = beam->ray.origin + beam->ray.direction * isect->shapeIntersection.distance;
    }
}

