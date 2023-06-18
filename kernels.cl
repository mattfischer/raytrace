struct QuadPrimitive {
    float3 position;
    float3 side1;
    float3 side2;
    float3 normal;
};

struct SpherePrimitive {
    float3 position;
    float radius;
};

enum PrimitiveType {
    PrimitiveTypeNone,
    PrimitiveTypeQuad,
    PrimitiveTypeSphere
};

struct Primitive {
    enum PrimitiveType type;
    uintptr_t primitive;
    union {
        struct QuadPrimitive quad;
        struct SpherePrimitive sphere;
    };
};

struct Scene {
    int numPrimitives;
    global struct Primitive *primitives;
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
};

float length2(float3 v)
{
    return dot(v, v);
}

bool intersectQuad(global struct Ray *ray, global struct QuadPrimitive *quad, global struct ShapeIntersection *shapeIntersection)
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

bool intersectSphere(global struct Ray *ray, global struct SpherePrimitive *sphere, global struct ShapeIntersection *shapeIntersection)
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

bool intersectPrimitive(global struct Ray *ray, global struct Primitive *primitive, global struct ShapeIntersection *shapeIntersection)
{
    switch(primitive->type) {
    case PrimitiveTypeQuad:
        return intersectQuad(ray, &primitive->quad, shapeIntersection);
    
    case PrimitiveTypeSphere:
        return intersectSphere(ray, &primitive->sphere, shapeIntersection);
    
    default:
        return false;
    }
}

kernel void intersectRays(global struct Scene *scene, global struct Item *items)
{
    int id = (int)get_global_id(0);
    global struct Item *item = &items[id];
    item->shapeIntersection.distance = MAXFLOAT;
    item->shapeIntersection.primitive = 0;
    
    for(int i=0; i<scene->numPrimitives; i++) {
        if(intersectPrimitive(&item->ray, &scene->primitives[i], &item->shapeIntersection)) {
            item->shapeIntersection.primitive = scene->primitives[i].primitive;
        }
    }
}
