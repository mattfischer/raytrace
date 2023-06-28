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

struct Primitive {
    struct Shape shape;
    uintptr_t primitive;    
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
    struct Ray shadowRay;
    struct ShapeIntersection shadowShapeIntersection;
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

kernel void intersectRays(global struct Scene *scene, global struct Item *items)
{
    int id = (int)get_global_id(0);
    global struct Item *item = &items[id];
    item->shapeIntersection.distance = MAXFLOAT;
    item->shapeIntersection.primitive = 0;
    
    for(int i=0; i<scene->numPrimitives; i++) {
        if(intersectShape(&item->ray, &scene->primitives[i].shape, &item->shapeIntersection)) {
            item->shapeIntersection.primitive = scene->primitives[i].primitive;
        }
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
