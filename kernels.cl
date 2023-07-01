struct Bivector {
    float3 u;
    float3 v;
};

struct QuadShape {
    float3 position;
    float3 side1;
    float3 side2;
    float3 normal;
};

struct SphereShape {
    float3 position;
    float radius;
};

enum ShapeType {
    ShapeTypeNone,
    ShapeTypeQuad,
    ShapeTypeSphere
};

struct Shape {
    enum ShapeType type;
    union {
        struct QuadShape quad;
        struct SphereShape sphere;
    };
};

struct SolidAlbedo {
    float3 color;
};

enum AlbedoType {
    AlbedoTypeSolid,
    AlbedoTypeTexture
};

struct Albedo {
    enum AlbedoType type;
    union {
        struct SolidAlbedo solid;
    };
};

struct Surface {
    float3 radiance;
    struct Albedo albedo;
};

struct Primitive {
    struct Shape shape;
    struct Surface surface;
    uintptr_t primitive;    
};

struct PointLight {
    float3 position;
    float3 radiance;
};

struct Camera {
    float3 position;
    float3 direction;
    struct Bivector imagePlane;
    float imageSize;
    float focalLength;
    float apertureSize;
};

struct Scene {
    int numPrimitives;
    global struct Primitive *primitives;
    int numAreaLights;
    global struct Primitive * global *areaLights;
    int numPointLights;
    global struct PointLight *pointLights;
    float3 skyRadiance;
    struct Camera camera;
};

struct Ray {
    float3 origin;
    float3 direction;
};

struct Beam {
    struct Ray ray;
    struct Bivector originDifferential;
    struct Bivector directionDifferential;
};

struct ShapeIntersection {
    float distance;
    float3 normal;
};

struct Intersection {
    struct ShapeIntersection shapeIntersection;
    global struct Primitive *primitive;
    global struct Beam *beam;
    float3 point;
};

struct Settings {
    int width;
    int height;
};

struct Item {
    struct Beam beam;
    struct Intersection isect;
    struct Ray shadowRay;
    struct Intersection shadowIsect;
    bool specularBounce;
    int generation;
    float pdf;
    float3 throughput;
    float3 radiance;
    float random[4];
    int lightIndex;
    int currentPixel;
    int x;
    int y;
};

float length2(float3 v)
{
    return dot(v, v);
}

bool intersectQuad(global struct Ray *ray, global struct QuadShape *quad, global struct ShapeIntersection *shapeIntersection)
{
    float distance = dot(ray->origin - quad->position, quad->normal) / dot(ray->direction, -quad->normal);
    if(distance >= 0 && distance < shapeIntersection->distance) {
        float3 point = ray->origin + ray->direction * distance;
        float u = dot(point - quad->position, quad->side1) / length2(quad->side1);
        float v = dot(point - quad->position, quad->side2) / length2(quad->side2);
        if(u >= 0 && u <= 1 && v >= 0 && v <= 1) {
            shapeIntersection->distance = distance;
            shapeIntersection->normal = quad->normal;
            return true;
        } 
    }

    return false;
}

bool intersectSphere(global struct Ray *ray, global struct SphereShape *sphere, global struct ShapeIntersection *shapeIntersection)
{
    float a, b, c;
    float disc;

    a = length2(ray->direction);
    b = 2 * dot(ray->origin - sphere->position, ray->direction);
    c = length2(ray->origin - sphere->position) - sphere->radius * sphere->radius;

    disc = b * b - 4 * a * c;
    if(disc >= 0) {
        float distance = (-b - sqrt(disc)) / (2 * a);
        if(distance >= 0 && distance < shapeIntersection->distance) {
            shapeIntersection->distance = distance;
            float3 point = ray->origin + ray->direction * distance;
            shapeIntersection->normal = (point - sphere->position) / sphere->radius;
            return true;
        }

        distance = (-b + sqrt(disc)) / (2 * a);
        if(distance >= 0 && distance < shapeIntersection->distance) {
            shapeIntersection->distance = distance;
            float3 point = ray->origin + ray->direction * distance;
            shapeIntersection->normal = (point - sphere->position) / sphere->radius;
            return true;
        }
    }

    return false;
}

bool intersectShape(global struct Ray *ray, global struct Shape *shape, global struct ShapeIntersection *shapeIntersection)
{
    switch(shape->type) {
    case ShapeTypeQuad:
        return intersectQuad(ray, &shape->quad, shapeIntersection);
    
    case ShapeTypeSphere:
        return intersectSphere(ray, &shape->sphere, shapeIntersection);
    
    default:
        return false;
    }
}

float shapeSamplePdfQuad(global struct QuadShape *quad, float3 point)
{
    float surfaceArea = length(cross(quad->side1, quad->side2));
    return 1.0f / surfaceArea;
}

float shapeSamplePdf(global struct Shape *shape, float3 point)
{
    switch(shape->type) {
    case ShapeTypeQuad:
        return shapeSamplePdfQuad(&shape->quad, point);
    
    default:
        return 0;
    }
}

bool shapeSampleQuad(global struct QuadShape *quad, float2 random, float3 *point, float3 *normal, float *pdf)
{
    *point = quad->position + quad->side1 * random.x + quad->side2 * random.y;
    *normal = quad->normal;

    float surfaceArea = length(cross(quad->side1, quad->side2));
    *pdf = 1.0f / surfaceArea;

    return true;
}

bool shapeSample(global struct Shape *shape, float2 random, float3 *point, float3 *normal, float *pdf)
{
    switch(shape->type) {
    case ShapeTypeQuad:
        return shapeSampleQuad(&shape->quad, random, point, normal, pdf);
    
    default:
        return false;
    }
}

float3 facingNormal(global struct Intersection *isect)
{
    if(dot(isect->shapeIntersection.normal, isect->beam->ray.direction) > 0) {
        return -isect->shapeIntersection.normal;
    } else {
        return isect->shapeIntersection.normal;
    }
}

float surfacePdf(global struct Intersection *isect, float3 dirIn)
{
    float3 dirOut = -isect->beam->ray.direction;
    float3 nrmFacing = facingNormal(isect);

    float cosTheta = max(dot(dirIn, nrmFacing), 0.0f);
    return cosTheta / 3.14f;
}

float3 surfaceReflected(global struct Intersection *isect, float3 dirIn)
{
    float3 dirOut = -isect->beam->ray.direction;
    float3 nrmFacing = facingNormal(isect);
    return isect->primitive->surface.albedo.solid.color / 3.14f;        
}

void createPixelBeam(global struct Camera *camera, float2 imagePoint, int width, int height, float2 aperturePoint, global struct Beam *beam)
{
    float cx = (2 * imagePoint.x - width) / width;
    float cy = (2 * imagePoint.y - height) / width;
    float2 imagePointTransformed = (float2)(cx, -cy);

    float3 direction = camera->direction + (camera->imagePlane.u * imagePointTransformed.x + camera->imagePlane.v * imagePointTransformed.y) * camera->imageSize;
    float len = length(direction);
    direction = direction / len;

    float3 p = camera->position + direction * camera->focalLength;
    float r = sqrt(aperturePoint.x);
    float phi = 2 * 3.14f * aperturePoint.y;
    float2 apertureDiscPoint = (float2)(r * cos(phi), r * sin(phi));
    float3 q = camera->position + (camera->imagePlane.u * apertureDiscPoint.x + camera->imagePlane.v * apertureDiscPoint.y) * camera->apertureSize;
    direction = normalize(p - q);

    beam->ray.origin = q;
    beam->ray.direction = direction;
    
    float pixelSize = 2.0f / width;

    beam->originDifferential.u = (float3)(0, 0, 0);
    beam->originDifferential.v = (float3)(0, 0, 0);
    beam->directionDifferential.u = camera->imagePlane.u * pixelSize / len;
    beam->directionDifferential.v = camera->imagePlane.v * pixelSize / len;
}

kernel void generateCameraRays(global struct Scene *scene, global struct Settings *settings, global struct Item *items)
{
    int id = (int)get_global_id(0);
    global struct Item *item = &items[id];

    item->y = (item->currentPixel / settings->width) % settings->height;
    item->x = item->currentPixel % settings->width;

    float2 imagePoint = (float2)(item->x, item->y) + (float2)(item->random[0], item->random[1]);
    float2 aperturePoint = (float2)(item->random[2], item->random[3]);
    createPixelBeam(&scene->camera, imagePoint, settings->width, settings->height, aperturePoint, &item->beam);
    item->specularBounce = false;
    item->generation = 0;
    item->radiance = (float3)(0, 0, 0);
    item->throughput = (float3)(1, 1, 1);
}

kernel void intersectRays(global struct Scene *scene, global struct Item *items)
{
    int id = (int)get_global_id(0);
    global struct Item *item = &items[id];
    item->isect.shapeIntersection.distance = MAXFLOAT;
    item->isect.primitive = 0;
    
    for(int i=0; i<scene->numPrimitives; i++) {
        global struct Primitive *primitive = &scene->primitives[i];
        if(intersectShape(&item->beam.ray, &primitive->shape, &item->isect.shapeIntersection)) {
            item->isect.primitive = primitive;
            item->isect.beam = &item->beam;
            item->isect.point = item->beam.ray.origin + item->beam.ray.direction * item->isect.shapeIntersection.distance;
            float3 rad2 = primitive->surface.radiance;
            float misWeight = 1.0f;
            if(length(rad2) > 0 && !item->specularBounce && item->generation > 0) {
                float dot2 = fabs(dot(item->isect.shapeIntersection.normal, item->beam.ray.direction));
                float pdfArea = item->pdf * dot2 / (item->isect.shapeIntersection.distance * item->isect.shapeIntersection.distance);
                float pdfLight = shapeSamplePdf(&primitive->shape, item->isect.point);
                misWeight = pdfArea * pdfArea / (pdfArea * pdfArea + pdfLight * pdfLight);
            }

            item->radiance = rad2 * item->throughput * misWeight;        
        
            int totalLights = scene->numAreaLights + scene->numPointLights;
            item->lightIndex = (int)floor(item->random[0] * totalLights);                    
        }
    }

    if(item->isect.primitive == 0) {
        float3 rad2 = scene->skyRadiance;
        item->radiance = rad2 * item->throughput;
    }
}

kernel void directLightArea(global struct Scene *scene, global struct Item *items)
{
    int id = (int)get_global_id(0);
    global struct Item *item = &items[id];
    item->shadowIsect.shapeIntersection.distance = MAXFLOAT;
    item->shadowIsect.primitive = 0;
    item->radiance = (float3)(0, 0, 0);

    global struct Intersection *isect = &item->isect;
    float3 nrmFacing = facingNormal(isect);
    float3 pntOffset = isect->point + nrmFacing * 0.01f;

    global struct Primitive *light = scene->areaLights[item->lightIndex];
    
    float2 rand = (float2)(item->random[0], item->random[1]);
    float3 pnt2;
    float3 nrm2;
    float pdf;
    if(shapeSample(&light->shape, rand, &pnt2, &nrm2, &pdf)) {
        float3 dirIn = pnt2 - pntOffset;
        float d = length(dirIn);
        dirIn = dirIn / d;
        float dot2 = fabs(dot(dirIn, nrm2));
        float dt = dot(dirIn, nrmFacing);
        if(dt > 0) {    
            item->shadowRay.origin = pntOffset;
            item->shadowRay.direction = dirIn;

            for(int i=0; i<scene->numPrimitives; i++) {
                if(intersectShape(&item->shadowRay, &scene->primitives[i].shape, &item->shadowIsect.shapeIntersection)) {
                    item->shadowIsect.primitive = &scene->primitives[i];
                }
            }
            
            if(item->shadowIsect.primitive == light) {
                float3 rad2 = light->surface.radiance;
                float3 irad = rad2 * dot2 * dt / (d * d * pdf);
                float pdfBrdf = surfacePdf(isect, dirIn) * dot2 / (d * d);
                float misWeight = pdf * pdf / (pdf * pdf + pdfBrdf * pdfBrdf);
                float3 rad = irad * surfaceReflected(isect, dirIn);
                item->radiance = rad * item->throughput * misWeight;
            }
        }
    }
}

kernel void directLightPoint(global struct Scene *scene, global struct Item *items)
{
    int id = (int)get_global_id(0);
    global struct Item *item = &items[id];
    item->shadowIsect.shapeIntersection.distance = MAXFLOAT;
    item->shadowIsect.primitive = 0;
    item->radiance = (float3)(0, 0, 0);

    global struct Intersection *isect = &item->isect;
    float3 nrmFacing = facingNormal(isect);
    float3 pntOffset = isect->point + nrmFacing * 0.01f;
    global struct PointLight *pointLight = &scene->pointLights[item->lightIndex];

    float3 dirIn = pointLight->position - pntOffset;
    float d = length(dirIn);
    dirIn = dirIn / d;

    float dt = dot(dirIn, nrmFacing);
    if(dt > 0) {
        item->shadowRay.origin = pntOffset;
        item->shadowRay.direction = dirIn;

        for(int i=0; i<scene->numPrimitives; i++) {
            if(intersectShape(&item->shadowRay, &scene->primitives[i].shape, &item->shadowIsect.shapeIntersection)) {
                item->shadowIsect.primitive = &scene->primitives[i];
            }
        }

        if(item->shadowIsect.primitive == 0 || item->shadowIsect.shapeIntersection.distance >= 0) {
            float3 irad = pointLight->radiance * dt / (d * d);
            float3 rad = irad * surfaceReflected(isect, dirIn);
            item->radiance = rad * item->throughput;
        }
    }
}
