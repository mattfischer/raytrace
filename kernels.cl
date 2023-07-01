typedef float3 Point;
typedef float3 Vector;
typedef float3 Normal;
typedef float3 Color;
typedef float3 Radiance;

typedef struct {
    Vector u;
    Vector v;
} Bivector;

typedef struct {
    Point position;
    Vector side1;
    Vector side2;
    Normal normal;
} QuadShape;

typedef struct {
    Point position;
    float radius;
} SphereShape;

typedef enum {
    ShapeTypeNone,
    ShapeTypeQuad,
    ShapeTypeSphere
} ShapeType;

typedef struct {
    ShapeType type;
    union {
        QuadShape quad;
        SphereShape sphere;
    };
} Shape;

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
    Point origin;
    Vector direction;
} Ray;

typedef struct {
    Ray ray;
    Bivector originDifferential;
    Bivector directionDifferential;
} Beam;

typedef struct {
    float distance;
    Normal normal;
} ShapeIntersection;

typedef struct {
    ShapeIntersection shapeIntersection;
    Primitive *primitive;
    Beam *beam;
    Point point;
} Intersection;

typedef struct {
    int width;
    int height;
    int minSamples;
} Settings;

typedef struct {
    Beam beam;
    Intersection isect;
    bool specularBounce;
    int generation;
    float pdf;
    Color throughput;
    Radiance radiance;
    int lightIndex;
    int x;
    int y;
} Item;

typedef struct {
    global int *data;
} WorkQueue;

typedef struct {
    WorkQueue generateCameraRayQueue;
    WorkQueue intersectRaysQueue;
    WorkQueue directLightAreaQueue;
    WorkQueue directLightPointQueue;
    WorkQueue extendPathQueue;
    WorkQueue commitRadianceQueue;
} Queues;

void queueAddItem(WorkQueue *queue, int key)
{
    int write = atomic_inc(&queue->data[1]);
    
    queue->data[write + 2] = key;
}

int queueGetNextKey(WorkQueue *queue)
{
    int read = atomic_inc(&queue->data[0]);

    return queue->data[read + 2];
}

float length2(float3 v)
{
    return dot(v, v);
}

bool intersectQuad(Ray *ray, QuadShape *quad, ShapeIntersection *shapeIntersection)
{
    float distance = dot(ray->origin - quad->position, quad->normal) / dot(ray->direction, -quad->normal);
    if(distance >= 0 && distance < shapeIntersection->distance) {
        Point point = ray->origin + ray->direction * distance;
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

bool intersectSphere(Ray *ray, SphereShape *sphere, ShapeIntersection *shapeIntersection)
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
            Point point = ray->origin + ray->direction * distance;
            shapeIntersection->normal = (point - sphere->position) / sphere->radius;
            return true;
        }

        distance = (-b + sqrt(disc)) / (2 * a);
        if(distance >= 0 && distance < shapeIntersection->distance) {
            shapeIntersection->distance = distance;
            Point point = ray->origin + ray->direction * distance;
            shapeIntersection->normal = (point - sphere->position) / sphere->radius;
            return true;
        }
    }

    return false;
}

bool intersectShape(Ray *ray, Shape *shape, ShapeIntersection *shapeIntersection)
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

float shapeSamplePdfQuad(QuadShape *quad, Point point)
{
    float surfaceArea = length(cross(quad->side1, quad->side2));
    return 1.0f / surfaceArea;
}

float shapeSamplePdf(Shape *shape, Point point)
{
    switch(shape->type) {
    case ShapeTypeQuad:
        return shapeSamplePdfQuad(&shape->quad, point);
    
    default:
        return 0;
    }
}

bool shapeSampleQuad(QuadShape *quad, float2 random, Point *point, Normal *normal, float *pdf)
{
    *point = quad->position + quad->side1 * random.x + quad->side2 * random.y;
    *normal = quad->normal;

    float surfaceArea = length(cross(quad->side1, quad->side2));
    *pdf = 1.0f / surfaceArea;

    return true;
}

bool shapeSample(Shape *shape, float2 random, Point *point, Normal *normal, float *pdf)
{
    switch(shape->type) {
    case ShapeTypeQuad:
        return shapeSampleQuad(&shape->quad, random, point, normal, pdf);
    
    default:
        return false;
    }
}

Normal facingNormal(Intersection *isect)
{
    if(dot(isect->shapeIntersection.normal, isect->beam->ray.direction) > 0) {
        return -isect->shapeIntersection.normal;
    } else {
        return isect->shapeIntersection.normal;
    }
}

float surfacePdf(Intersection *isect, Vector dirIn)
{
    Vector dirOut = -isect->beam->ray.direction;
    Normal nrmFacing = facingNormal(isect);

    float cosTheta = max(dot(dirIn, nrmFacing), 0.0f);
    return cosTheta / 3.14f;
}

Color surfaceReflected(Intersection *isect, Vector dirIn)
{
    Vector dirOut = -isect->beam->ray.direction;
    Normal nrmFacing = facingNormal(isect);
    return isect->primitive->surface.albedo.solid.color / 3.14f;        
}

void sceneIntersect(Scene *scene, Beam *beam, Intersection *isect)
{
    isect->shapeIntersection.distance = MAXFLOAT;
    isect->primitive = 0;
    isect->beam = beam;

    for(int i=0; i<scene->numPrimitives; i++) {
        if(intersectShape(&beam->ray, &scene->primitives[i].shape, &isect->shapeIntersection)) {
            isect->primitive = &scene->primitives[i];
        }
    }

    if(isect->primitive != 0) {
        isect->point = beam->ray.origin + beam->ray.direction * isect->shapeIntersection.distance;
    }
}

Color surfaceSample(Intersection *isect, float2 random, Vector *dirIn, float *pdf, bool *pdfDelta)
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
    *pdf = surfacePdf(isect, *dirIn);
    *pdfDelta = false;
    return surfaceReflected(isect, *dirIn);
}
        
void createPixelBeam(Camera *camera, float2 imagePoint, int width, int height, float2 aperturePoint, Beam *beam)
{
    float cx = (2 * imagePoint.x - width) / width;
    float cy = (2 * imagePoint.y - height) / width;
    float2 imagePointTransformed = (float2)(cx, -cy);

    Vector direction = camera->direction + (camera->imagePlane.u * imagePointTransformed.x + camera->imagePlane.v * imagePointTransformed.y) * camera->imageSize;
    float len = length(direction);
    direction = direction / len;

    Point p = camera->position + direction * camera->focalLength;
    float r = sqrt(aperturePoint.x);
    float phi = 2 * 3.14f * aperturePoint.y;
    float2 apertureDiscPoint = (float2)(r * cos(phi), r * sin(phi));
    Point q = camera->position + (camera->imagePlane.u * apertureDiscPoint.x + camera->imagePlane.v * apertureDiscPoint.y) * camera->apertureSize;
    direction = normalize(p - q);

    beam->ray.origin = q;
    beam->ray.direction = direction;
    
    float pixelSize = 2.0f / width;

    beam->originDifferential.u = (float3)(0, 0, 0);
    beam->originDifferential.v = (float3)(0, 0, 0);
    beam->directionDifferential.u = camera->imagePlane.u * pixelSize / len;
    beam->directionDifferential.v = camera->imagePlane.v * pixelSize / len;
}

kernel void generateCameraRays(global Scene *scene, global Settings *settings, global Item *items, global float* random, global Queues *queues, global unsigned int *currentPixel)
{
    int key = queueGetNextKey(&queues->generateCameraRayQueue);
    Item *item = &items[key];

    unsigned int cp = atomic_inc(currentPixel);

    int sample = cp / (settings->width * settings->height);
    if(sample >= settings->minSamples) {
        return;
    }

    item->y = (cp / settings->width) % settings->height;
    item->x = cp % settings->width;

    float *r = random + key * 10;

    float2 imagePoint = (float2)(item->x, item->y) + (float2)(r[0], r[1]);
    float2 aperturePoint = (float2)(r[2], r[3]);
    createPixelBeam(&scene->camera, imagePoint, settings->width, settings->height, aperturePoint, &item->beam);
    item->specularBounce = false;
    item->generation = 0;
    item->radiance = (float3)(0, 0, 0);
    item->throughput = (float3)(1, 1, 1);

    queueAddItem(&queues->intersectRaysQueue, key);
}

kernel void intersectRays(global Scene *scene, global Item *items, global float *random, global Queues *queues)
{
    int key = queueGetNextKey(&queues->intersectRaysQueue);
    Item *item = &items[key];

    sceneIntersect(scene, &item->beam, &item->isect);

    if(item->isect.primitive != 0) {
        Radiance rad2 = item->isect.primitive->surface.radiance;
        float misWeight = 1.0f;
        if(length(rad2) > 0 && !item->specularBounce && item->generation > 0) {
            Normal nrmFacing = facingNormal(&item->isect);
            float dot2 = -dot(nrmFacing, item->beam.ray.direction);
            float d = item->isect.shapeIntersection.distance;
            float pdfArea = item->pdf * dot2 / (d * d);
            float pdfLight = shapeSamplePdf(&item->isect.primitive->shape, item->isect.point);
            misWeight = pdfArea * pdfArea / (pdfArea * pdfArea + pdfLight * pdfLight);
        }

        item->radiance += rad2 * item->throughput * misWeight;        
    
        int totalLights = scene->numAreaLights + scene->numPointLights;
        float *r = random + key * 10;
        int lightIndex = (int)floor(r[4] * totalLights);

        if(lightIndex < scene->numAreaLights) {
            item->lightIndex = lightIndex;
            queueAddItem(&queues->directLightAreaQueue, key);
        } else {
            item->lightIndex = lightIndex - scene->numAreaLights;
            queueAddItem(&queues->directLightPointQueue, key);
        }
    }

    if(item->isect.primitive == 0) {
        Radiance rad2 = scene->skyRadiance;
        item->radiance += rad2 * item->throughput;
        queueAddItem(&queues->commitRadianceQueue, key);
    }
}

kernel void directLightArea(global Scene *scene, global Item *items, global float *random, global Queues *queues)
{
    int key = queueGetNextKey(&queues->directLightAreaQueue);
    Item *item = &items[key];

    Intersection *isect = &item->isect;
    Normal nrmFacing = facingNormal(isect);
    Point pntOffset = isect->point + nrmFacing * 0.01f;

    Primitive *light = scene->areaLights[item->lightIndex];
    
    float *r = random + key * 10;

    float2 rand = (float2)(r[5], r[6]);
    Point pnt2;
    Normal nrm2;
    float pdf;
    if(shapeSample(&light->shape, rand, &pnt2, &nrm2, &pdf)) {
        Vector dirIn = pnt2 - pntOffset;
        float d = length(dirIn);
        dirIn = dirIn / d;
        float dot2 = fabs(dot(dirIn, nrm2));
        float dt = dot(dirIn, nrmFacing);
        if(dt > 0) {    
            Beam shadowBeam;
            shadowBeam.ray.origin = pntOffset;
            shadowBeam.ray.direction = dirIn;

            Intersection shadowIsect;
            sceneIntersect(scene, &shadowBeam, &shadowIsect);

            if(shadowIsect.primitive == light) {
                Radiance rad2 = light->surface.radiance;
                Radiance irad = rad2 * dot2 * dt / (d * d * pdf);
                float pdfBrdf = surfacePdf(isect, dirIn) * dot2 / (d * d);
                float misWeight = pdf * pdf / (pdf * pdf + pdfBrdf * pdfBrdf);
                Radiance rad = irad * surfaceReflected(isect, dirIn);
                item->radiance += rad * item->throughput * misWeight;
            }
        }
    }

    queueAddItem(&queues->extendPathQueue, key);
}

kernel void directLightPoint(global Scene *scene, global Item *items, global Queues *queues)
{
    int key = queueGetNextKey(&queues->directLightPointQueue);
    Item *item = &items[key];

    Intersection *isect = &item->isect;
    Normal nrmFacing = facingNormal(isect);
    Point pntOffset = isect->point + nrmFacing * 0.01f;
    PointLight *pointLight = &scene->pointLights[item->lightIndex];

    Vector dirIn = pointLight->position - pntOffset;
    float d = length(dirIn);
    dirIn = dirIn / d;

    float dt = dot(dirIn, nrmFacing);
    if(dt > 0) {
        Beam shadowBeam;
        shadowBeam.ray.origin = pntOffset;
        shadowBeam.ray.direction = dirIn;

        Intersection shadowIsect;
        sceneIntersect(scene, &shadowBeam, &shadowIsect);

        if(shadowIsect.primitive == 0 || shadowIsect.shapeIntersection.distance >= 0) {
            Radiance irad = pointLight->radiance * dt / (d * d);
            Radiance rad = irad * surfaceReflected(isect, dirIn);
            item->radiance += rad * item->throughput;
        }
    }

    queueAddItem(&queues->extendPathQueue, key);
}

kernel void extendPath(global Scene *scene, global Item *items, global float *random, global Queues *queues)
{
    int key = queueGetNextKey(&queues->extendPathQueue);
    Item *item = &items[key];
    Intersection *isect = &item->isect;
    Normal nrmFacing = facingNormal(isect);
    
    Vector dirIn;
    float pdf;
    bool pdfDelta;
    
    float *r = random + key * 10;

    float2 rand = (float2)(r[7], r[8]);
    Color reflected = surfaceSample(isect, rand, &dirIn, &pdf, &pdfDelta);
    float dt = dot(dirIn, nrmFacing);
    Point pntOffset = isect->point + nrmFacing * 0.01f;
    
    item->pdf = pdf;
    item->specularBounce = pdfDelta;
    if(dt > 0) {
        item->throughput = item->throughput * reflected * dt / pdf;

        float threshold = 0.0f;
        float roulette = r[9];
        if(item->generation == 0) {
            threshold = 1.0f;
        } else if(item->generation < 10) {
            float tmax = max(item->throughput.x, max(item->throughput.y, item->throughput.z));
            threshold = min(1.0f, tmax);
        }

        if(roulette < threshold) {
            item->beam.ray.origin = pntOffset;
            item->beam.ray.direction = dirIn;
            item->throughput = item->throughput / threshold;
            item->generation++;
            queueAddItem(&queues->intersectRaysQueue, key);
        } else {
            queueAddItem(&queues->commitRadianceQueue, key);
        }
    } else {
        queueAddItem(&queues->commitRadianceQueue, key);
    }
}