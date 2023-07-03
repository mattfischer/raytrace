typedef struct {
    Radiance radiance;
    Albedo albedo;
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
    Vector dirOut = -isect->beam->ray.direction;
    Normal nrmFacing = facingNormal(isect);

    float cosTheta = max(dot(dirIn, nrmFacing), 0.0f);
    return cosTheta / 3.14f;
}

Color Surface_reflected(Intersection *isect, Vector dirIn)
{
    Vector dirOut = -isect->beam->ray.direction;
    Normal nrmFacing = facingNormal(isect);
    return isect->primitive->surface.albedo.solid.color / 3.14f;        
}

Color Surface_sample(Intersection *isect, float2 random, Vector *dirIn, float *pdf, bool *pdfDelta)
{
    Vector dirOut = -isect->beam->ray.direction;
    Normal nrmFacing = facingNormal(isect);
    
    float phi = 2 * 3.14f * random.x;
    float theta = asin(sqrt(random.y));

    Vector x;
    Vector y;
    Vector z = nrmFacing;
    if(fabs(nrmFacing.z) > fabs(nrmFacing.x) && fabs(nrmFacing.z) > fabs(nrmFacing.y)) {
        x = (float3)(1, 0, 0);
        y = (float3)(0, 1, 0);
    } else if(fabs(nrmFacing.x) > fabs(nrmFacing.y) && fabs(nrmFacing.x) > fabs(nrmFacing.z)) {
        x = (float3)(0, 1, 0);
        y = (float3)(0, 0, 1);
    } else {
        x = (float3)(1, 0, 0);
        y = (float3)(0, 0, 1);
    }
    
    x = normalize(x - z * dot(x, z));
    y = normalize(y - z * dot(y, z));

    *dirIn = x * cos(phi) * cos(theta) + y * sin(phi) * cos(theta) + z * sin(theta);
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

