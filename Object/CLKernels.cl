typedef struct {
    Radiance radiance;
    Albedo albedo;
    int numBrdfs;
    Brdf *brdfs;
    bool opaque;
    float transmitIor;
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
    Normal normal;
    Normal facingNormal;
    Bivector2D surfaceProjection;
} Intersection;

float Surface_pdf(Intersection *isect, Vector dirIn)
{
    Surface *surf = &isect->primitive->surface;
    Vector dirOut = -isect->beam->ray.direction;
    Normal nrmFacing = isect->facingNormal;

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
    Normal nrmFacing = isect->facingNormal;
    Color albedo = Albedo_color(&surf->albedo, isect->shapeIntersection.surfacePoint, &isect->surfaceProjection);

    Color col = (Color)(0, 0, 0);
    Color colTransmit = (Color)(1, 1, 1);
    for(int i=0; i<surf->numBrdfs; i++) {
        col = col + colTransmit * Brdf_reflected(&surf->brdfs[i], dirIn, nrmFacing, dirOut, albedo);
        colTransmit = colTransmit * Brdf_transmitted(&surf->brdfs[i], dirIn, nrmFacing, albedo);
    }

    return col;
}

Color Surface_transmitted(Intersection *isect, Vector dirIn)
{
    Surface *surf = &isect->primitive->surface;
    Normal nrmFacing = isect->facingNormal;
    Color albedo = Albedo_color(&surf->albedo, isect->shapeIntersection.surfacePoint, &isect->surfaceProjection);

    Color colTransmit = (Color)(1, 1, 1);
    for(int i=0; i<surf->numBrdfs; i++) {
        colTransmit = colTransmit * Brdf_transmitted(&surf->brdfs[i], dirIn, -nrmFacing, albedo);
    }

    return colTransmit;
}

Color Surface_sample(Intersection *isect, float4 random, Vector *dirIn, float *pdf, bool *pdfDelta)
{
    Surface *surf = &isect->primitive->surface;
    Vector dirOut = -isect->beam->ray.direction;
    Normal nrmFacing = isect->facingNormal;

    float transmitThreshold = 0;
    if(!surf->opaque) {       
        bool reverse = (dot(isect->shapeIntersection.normal, dirOut) < 0);
        float ratio = 1.0f / surf->transmitIor;
        if(reverse) {
            ratio = 1.0f / ratio;
        }

        float c1 = dot(dirOut, nrmFacing);
        float c2 = sqrt(1.0f - ratio * ratio * (1.0f - c1 * c1));

        *dirIn = nrmFacing * (ratio * c1 - c2) - dirOut * ratio;
        Color throughput = Surface_transmitted(isect, -dirOut);
        transmitThreshold = min(1.0f, max(max(throughput.x, throughput.y), throughput.z));
        float roulette = random.x;

        if(roulette < transmitThreshold) {
            *pdf = 1.0f;
            *pdfDelta = true;
            return Surface_transmitted(isect, *dirIn) / (dot(dirOut, nrmFacing) * transmitThreshold);
        }
    }

    int idx = 0;
    if(surf->numBrdfs > 1) {
        idx = (int)floor(surf->numBrdfs * random.y);
    }

    *dirIn = Brdf_sample(&surf->brdfs[idx], random.zw, nrmFacing, dirOut);
    *pdf = Surface_pdf(isect, *dirIn);
    *pdfDelta = false;
    return Surface_reflected(isect, *dirIn);
}

void Scene_intersect(Scene *scene, Beam *beam, Intersection *isect)
{
    isect->shapeIntersection.distance = MAXFLOAT;
    isect->primitive = NULL;
    isect->beam = beam;

    for(int i=0; i<scene->numPrimitives; i++) {
        if(Shape_intersect(&beam->ray, &scene->primitives[i].shape, &isect->shapeIntersection)) {
            isect->primitive = &scene->primitives[i];
        }
    }

    if(isect->primitive != 0) {
        isect->point = beam->ray.origin + beam->ray.direction * isect->shapeIntersection.distance;
        isect->normal = isect->shapeIntersection.normal;
        bool reverse = (dot(isect->normal, isect->beam->ray.direction) > 0);
        isect->facingNormal = isect->normal * (reverse ? -1 : 1);       

        Bivector projection;
        Beam_project(beam, isect->shapeIntersection.distance, isect->normal, &projection);
        Vector v = cross(isect->shapeIntersection.tangent.u, isect->shapeIntersection.tangent.v);
        v = v / dot(v, v);
        Vector2D du = (Vector2D)(dot(cross(projection.u, isect->shapeIntersection.tangent.v), v),
                                 dot(cross(projection.u, isect->shapeIntersection.tangent.u), v));
        Vector2D dv = (Vector2D)(dot(cross(projection.v, isect->shapeIntersection.tangent.v), v),
                                 dot(cross(projection.u, isect->shapeIntersection.tangent.v), v));
        isect->surfaceProjection.u = du;
        isect->surfaceProjection.v = dv;
    }
}

