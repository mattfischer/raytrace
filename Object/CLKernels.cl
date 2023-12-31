typedef struct {
    Texture texture;
} NormalMap;

typedef struct {
    Radiance radiance;
    Albedo albedo;
    int numBrdfs;
    Brdf *brdfs;
    bool opaque;
    float transmitIor;
    NormalMap *normalMap;
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
    BVHNode *bvh;
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

Normal NormalMap_perturbNormal(NormalMap *normalMap, Point2D surfacePoint, Bivector2D *surfaceProjection, Normal normal, Bivector *tangent)
{
    float2 value = Texture_sample2(&normalMap->texture, surfacePoint, surfaceProjection);
    Vector offset = tangent->u * value.x + tangent->v * value.y;
    return normalize(normal + offset);
}

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

Color Surface_sample(Intersection *isect, Sampler *sampler, SamplerState *samplerState, Vector *dirIn, float *pdf, bool *pdfDelta)
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
        float roulette = Sampler_getValue(sampler, samplerState);

        if(roulette < transmitThreshold) {
            *pdf = 1.0f;
            *pdfDelta = true;
            return Surface_transmitted(isect, *dirIn) / (dot(dirOut, nrmFacing) * transmitThreshold);
        }
    }

    int idx = 0;
    if(surf->numBrdfs > 1) {
        idx = (int)floor(surf->numBrdfs * Sampler_getValue(sampler, samplerState));
    }

    *dirIn = Brdf_sample(&surf->brdfs[idx], sampler, samplerState, nrmFacing, dirOut);
    *pdf = Surface_pdf(isect, *dirIn);
    *pdfDelta = false;
    return Surface_reflected(isect, *dirIn);
}

void Scene_intersect(Scene *scene, Beam *beam, Intersection *isect, float maxDistance, bool closest)
{
    isect->shapeIntersection.distance = maxDistance;
    isect->primitive = NULL;
    isect->beam = beam;

    StackEntry stack[64];

    int n = 0;
    stack[n].nodeIndex = 0;
    stack[n].minDistance = 0;
    n++;

    do {
        n--;
        int nodeIndex = stack[n].nodeIndex;
        BVHNode *bvhNode = &scene->bvh[nodeIndex];
        float nodeMinimum = stack[n].minDistance;

        if(nodeMinimum > isect->shapeIntersection.distance) {
            continue;
        }

        if(bvhNode->index <= 0) {
            int index = -bvhNode->index;

            if(Shape_intersect(&beam->ray, &scene->primitives[index].shape, &isect->shapeIntersection, closest)) {
                isect->primitive = &scene->primitives[index];
                if(!closest) {
                    break;
                }
            }
        } else {
            int indices[2] = { nodeIndex + 1, bvhNode->index };
            float minDistances[2];
            float maxDistances[2];
            for(int i=0; i<2; i++) {
                minDistances[i] = MAXFLOAT;
                maxDistances[i] = -MAXFLOAT;
                BoundingVolume_intersect(&bvhNode->volume, &beam->ray, &minDistances[i], &maxDistances[i]);
            }

            for(int i=0; i<2; i++) {
                int j = (minDistances[0] >= minDistances[1]) ? i : 1 - i;
                if(maxDistances[j] > 0) {
                    stack[n].nodeIndex = indices[j];
                    stack[n].minDistance = minDistances[j];
                    n++;
                }
            }
        }
    } while(n > 0);

    if(isect->primitive != 0) {
        isect->point = beam->ray.origin + beam->ray.direction * isect->shapeIntersection.distance;
        isect->normal = isect->shapeIntersection.normal;

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

        NormalMap *normalMap = isect->primitive->surface.normalMap;
        if(normalMap) {
            isect->normal = NormalMap_perturbNormal(normalMap, isect->shapeIntersection.surfacePoint, &isect->surfaceProjection, isect->shapeIntersection.normal, &isect->shapeIntersection.tangent);
        }        
        bool reverse = (dot(isect->normal, isect->beam->ray.direction) > 0);
        isect->facingNormal = isect->normal * (reverse ? -1 : 1);
    }
}

