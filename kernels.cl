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

struct Surface {
    float3 radiance;
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

struct Scene {
    int numPrimitives;
    global struct Primitive *primitives;
    int numAreaLights;
    global struct Primitive **areaLights;
    int numPointLights;
    global struct PointLight *pointLights;
    float3 skyRadiance;
};

struct Ray {
    float3 origin;
    float3 direction;
};

struct ShapeIntersection {
    float distance;
    float3 normal;
    uintptr_t primitive;
};

struct Item {
    struct Ray ray;
    struct ShapeIntersection shapeIntersection;
    struct Ray shadowRay;
    struct ShapeIntersection shadowShapeIntersection;
    bool specularBounce;
    int generation;
    float pdf;
    float3 throughput;
    float3 radiance;
    float random[1];
    int lightIndex;
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

kernel void intersectRays(global struct Scene *scene, global struct Item *items)
{
    int id = (int)get_global_id(0);
    global struct Item *item = &items[id];
    item->shapeIntersection.distance = MAXFLOAT;
    item->shapeIntersection.primitive = 0;
    
    for(int i=0; i<scene->numPrimitives; i++) {
        global struct Primitive *primitive = &scene->primitives[i];
        if(intersectShape(&item->ray, &primitive->shape, &item->shapeIntersection)) {
            item->shapeIntersection.primitive = primitive->primitive;

            float3 point = item->ray.origin + item->ray.direction * item->shapeIntersection.distance;
            float3 rad2 = primitive->surface.radiance;
            float misWeight = 1.0f;
            if(length(rad2) > 0 && !item->specularBounce && item->generation > 0) {
                float dot2 = fabs(dot(item->shapeIntersection.normal, item->ray.direction));
                float pdfArea = item->pdf * dot2 / (item->shapeIntersection.distance * item->shapeIntersection.distance);
                float pdfLight = shapeSamplePdf(&primitive->shape, point);
                misWeight = pdfArea * pdfArea / (pdfArea * pdfArea + pdfLight * pdfLight);
            }

            item->radiance = rad2 * item->throughput * misWeight;        
        
            int totalLights = scene->numAreaLights + scene->numPointLights;
            item->lightIndex = (int)floor(item->random[0] * totalLights);                    
        }
    }

    if(item->shapeIntersection.primitive == 0) {
        float3 rad2 = scene->skyRadiance;
        item->radiance = rad2 * item->throughput;
    }
}

kernel void directLightArea(global struct Scene *scene, global struct Item *items)
{
    int id = (int)get_global_id(0);
    global struct Item *item = &items[id];
    item->shadowShapeIntersection.distance = MAXFLOAT;
    item->shadowShapeIntersection.primitive = 0;
    
    if(item->shadowRay.direction.x == 0 && item->shadowRay.direction.y == 0 && item->shadowRay.direction.z == 0) {
        return;
    }

    for(int i=0; i<scene->numPrimitives; i++) {
        if(intersectShape(&item->shadowRay, &scene->primitives[i].shape, &item->shadowShapeIntersection)) {
            item->shadowShapeIntersection.primitive = scene->primitives[i].primitive;
        }
    }
}

kernel void directLightPoint(global struct Scene *scene, global struct Item *items)
{
    int id = (int)get_global_id(0);
    global struct Item *item = &items[id];
    item->shadowShapeIntersection.distance = MAXFLOAT;
    item->shadowShapeIntersection.primitive = 0;
    
    if(item->shadowRay.direction.x == 0 && item->shadowRay.direction.y == 0 && item->shadowRay.direction.z == 0) {
        return;
    }

    for(int i=0; i<scene->numPrimitives; i++) {
        if(intersectShape(&item->shadowRay, &scene->primitives[i].shape, &item->shadowShapeIntersection)) {
            item->shadowShapeIntersection.primitive = scene->primitives[i].primitive;
        }
    }
}
